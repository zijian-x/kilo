#include <cctype>
#include <chrono>
#include <format>
#include <functional>
#include <numeric>
#include <type_traits>
#include <unistd.h>

#include "draw.hpp"
#include "editor.hpp"
#include "editor_keys.hpp"
#include "str.hpp"

static constexpr const char* KILO_VERS = "0.0.1";

using namespace char_seq;

void print_welcome(editor& ed, str& buf)
{
    auto msg = std::format("Kilo editor -- version {}", KILO_VERS);
    if (msg.size() > ed.screen_col())
        return;

    auto padding = (ed.screen_col() - msg.size() - 1) / 2;
    if (padding) {
        buf.push_back('~');
        --padding;
    }
    while (padding--)
        buf.push_back(' ');
    buf.append(msg.c_str());
}

void draw_status_msg_bar(editor& ed, str& buf)
{
    using std::chrono::time_point,
          std::chrono::system_clock,
          std::chrono::duration_cast,
          std::chrono::seconds;
    static auto diff_time = [](const time_point<system_clock>& ts) {
        return duration_cast<seconds>(system_clock::now() - ts).count();
    };

    buf.append(esc_seq::CLEAR_LINE);
    const auto& msg = ed.status_msg();
    if (!msg.content().empty() && diff_time(msg.timestamp()) < 2)
        buf.append(msg.content(), ed.screen_col());
}

void draw_statusbar(editor& ed, str& buf)
{
    buf.append(esc_seq::INVERT_COLOR);

    auto file_info = str(std::format("KILO_EDITOR | {} - {} lines{}",
                (ed.filename().empty() ?
                 "[No Name]" : ed.filename().c_str()),
                ed.rows().size(),
                (ed.dirty() ? " [+]" : "")).c_str());
    auto line_info = str(std::format("{}:{}",
                ed.c_row() + 1, ed.c_col() + 1).c_str());

    file_info.resize(ed.screen_col() - line_info.size(), ' ');
    buf.append(std::move(file_info));
    buf.append(std::move(line_info));

    buf.append(esc_seq::RESET_COLOR);
    buf.append(NEW_LINE);
}

void pad_hl(char hl, str& buf)
{
    char hl_code[16]{};
    auto len = snprintf(hl_code, sizeof(hl_code), "%c[%dm", editor_key::ESCAPE, hl);
    hl_code[len] = 0;
    buf.append(hl_code);
}

void draw_rows(editor& ed, str& buf)
{
    using std::begin, std::end;
    for (size_t i = 0; i < ed.screen_row(); ++i) {
        if (auto row_idx = i + ed.rowoff(); row_idx < ed.rows().size()) {
            const auto& row = ed.rows()[row_idx];
            const auto& render = row.render();
            auto start_index = std::min(ed.coloff(), render.size());
            auto max_len = std::min(render.size() - start_index, ed.screen_col());

            const auto& hl = row.hl();
            int prev_color = colors::DEFAULT;
            for (size_t j = 0; j < max_len; ++j) {
                if (hl[start_index + j] != prev_color)
                    pad_hl(hl[start_index + j], buf);
                buf.push_back(render[start_index + j]);
                prev_color = hl[start_index + j];
            }
            pad_hl(colors::DEFAULT, buf);
        } else if (ed.rows().empty() && i == ed.screen_row() >> 1) {
            print_welcome(ed, buf);
        } else {
            buf.push_back('~');
        }

        buf.append(esc_seq::CLEAR_LINE);
        buf.append(NEW_LINE);
    }
}

void reset_cursor_pos(editor& ed, str& buf)
{
    buf.append(std::format("\x1b[{:d};{:d}H",
                (ed.c_row() - ed.rowoff() + 1),
                (ed.r_col() - ed.coloff() + 1)).c_str());
}

void scroll(editor& ed)
{
    const auto& screen_row = ed.screen_row();
    const auto& screen_col = ed.screen_col();
    const auto& c_row = ed.c_row();
    const auto& c_col = ed.c_col();
    auto& r_col = ed.r_col();
    auto& rowoff = ed.rowoff();
    auto& coloff = ed.coloff();

    r_col = 0;
    if (c_row < ed.rows().size())
        ed.set_r_col();

    if (c_row < rowoff)
        rowoff = c_row;
    if (c_row >= rowoff + screen_row)
        rowoff = c_row - screen_row + 1;
    if (r_col < coloff)
        coloff = r_col;
    if (r_col >= coloff + screen_col)
        coloff = r_col - screen_col + 1;
}

void refresh_screen(editor& ed)
{
    scroll(ed);

    auto buf = str();
    buf.append(esc_seq::HIDE_CURSOR);
    buf.append(esc_seq::CLEAR_CURSOR_POS);
    draw_rows(ed, buf);
    draw_statusbar(ed, buf);
    draw_status_msg_bar(ed, buf);
    reset_cursor_pos(ed, buf);
    buf.append(esc_seq::SHOW_CURSOR);

    write(STDOUT_FILENO, buf.c_str(), buf.size());
}
