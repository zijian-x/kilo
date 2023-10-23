#include <cctype>
#include <chrono>
#include <format>
#include <functional>
#include <numeric>
#include <type_traits>
#include <unistd.h>

#include "draw.hpp"
#include "editor_state.hpp"
#include "editor_keys.hpp"
#include "str.hpp"

static constexpr const char* KILO_VERS = "0.0.1";

using namespace char_seq;

void print_welcome(editor_state& ed_state, str& buf)
{
    auto msg = std::format("Kilo editor -- version {}", KILO_VERS);
    if (msg.size() > ed_state.screen_col())
        return;

    auto padding = (ed_state.screen_col() - msg.size() - 1) / 2;
    if (padding) {
        buf.push_back('~');
        --padding;
    }
    while (padding--)
        buf.push_back(' ');
    buf.append(msg.c_str());
}

void draw_status_msg_bar(editor_state& ed_state, str& buf)
{
    using std::chrono::time_point,
          std::chrono::system_clock,
          std::chrono::duration_cast,
          std::chrono::seconds;
    static auto diff_time = [](const time_point<system_clock>& ts) {
        return duration_cast<seconds>(system_clock::now() - ts).count();
    };

    buf.append(esc_seq::CLEAR_LINE);
    const auto& msg = ed_state.status_msg();
    if (!msg.content().empty() && diff_time(msg.timestamp()) < 2)
        buf.append(msg.content(), ed_state.screen_col());
}

void draw_statusbar(editor_state& ed_state, str& buf)
{
    buf.append(esc_seq::INVERT_COLOR);

    auto file_info = str(std::format("KILO_EDITOR | {} - {} lines{}",
                (ed_state.filename().empty() ?
                 "[No Name]" : ed_state.filename().c_str()),
                ed_state.rows().size(),
                (ed_state.dirty() ? " [+]" : "")).c_str());
    auto line_info = str(std::format("{}:{}",
                ed_state.c_row() + 1, ed_state.c_col() + 1).c_str());

    file_info.resize(ed_state.screen_col() - line_info.size(), ' ');
    buf.append(std::move(file_info));
    buf.append(std::move(line_info));

    buf.append(esc_seq::RESET_COLOR);
    buf.append(NEW_LINE);
}

str render_row(const str& row)
{
    using std::begin, std::end;
    auto tab_cnt = std::count(begin(row), end(row), '\t');

    auto render = str();
    render.reserve(row.size() + static_cast<size_t>(tab_cnt * (TABSTOP - 1)) + 1);

    size_t idx = 0;
    for (auto c : row) {
        if (c == '\t') {
            auto cnt = TABSTOP - idx % TABSTOP;
            render.append(cnt, ' ');
            idx += cnt - 1;
        } else {
            render.push_back(c);
        }
        ++idx;
    }

    return render;
}

str highlight_render(const str& render)
{
    auto hl = str();
    hl.reserve(render.size());
    for (size_t i = 0; i < render.size(); ++i) {
        hl[i] = static_cast<char>
            ((std::isdigit(render[i]) ? colors::RED :colors::DEFAULT));
    }
    return hl;
}

void pad_hl(char hl, str& buf)
{
    char hl_code[16]{};
    auto len = snprintf(hl_code, sizeof(hl_code), "%c[%dm", editor_key::ESCAPE, hl);
    hl_code[len] = 0;
    buf.append(hl_code);
}

void draw_rows(editor_state& ed_state, str& buf)
{
    using std::begin, std::end;
    const auto& rows = ed_state.rows();
    for (size_t i = 0; i < ed_state.screen_row(); ++i) {
        if (auto row_idx = i + ed_state.rowoff(); row_idx < rows.size()) {
            const auto render = render_row(rows[row_idx]);
            auto start_index = std::min(ed_state.coloff(), render.size());
            auto max_len = std::min(render.size(), ed_state.screen_col());
            auto hl = highlight_render(render);

            int prev_color = colors::DEFAULT;
            for (size_t j = 0; j < max_len; ++j) {
                if (hl[j] != prev_color)
                    pad_hl(hl[j], buf);
                buf.push_back(render[start_index + j]);
                prev_color = hl[j];
            }
            pad_hl(colors::DEFAULT, buf);
        } else if (!rows.size() && i == ed_state.screen_row() >> 1) {
            print_welcome(ed_state, buf);
        } else {
            buf.push_back('~');
        }

        buf.append(esc_seq::CLEAR_LINE);
        buf.append(NEW_LINE);
    }
}

void reset_cursor_pos(editor_state& ed_state, str& buf)
{
    buf.append(std::format("\x1b[{:d};{:d}H",
                (ed_state.c_row() - ed_state.rowoff() + 1),
                (ed_state.r_col() - ed_state.coloff() + 1)).c_str());
}

void scroll(editor_state& ed_state)
{
    const auto& screen_row = ed_state.screen_row();
    const auto& screen_col = ed_state.screen_col();
    const auto& c_row = ed_state.c_row();
    const auto& c_col = ed_state.c_col();
    auto& r_col = ed_state.r_col();
    auto& rowoff = ed_state.rowoff();
    auto& coloff = ed_state.coloff();

    r_col = 0;
    if (c_row < ed_state.rows().size())
        ed_state.set_r_col();

    if (c_row < rowoff)
        rowoff = c_row;
    if (c_row >= rowoff + screen_row)
        rowoff = c_row - screen_row + 1;
    if (r_col < coloff)
        coloff = r_col;
    if (r_col >= coloff + screen_col)
        coloff = r_col - screen_col + 1;
}

void refresh_screen(editor_state& ed_state)
{
    scroll(ed_state);

    auto buf = str();
    buf.append(esc_seq::HIDE_CURSOR);
    buf.append(esc_seq::CLEAR_CURSOR_POS);
    draw_rows(ed_state, buf);
    draw_statusbar(ed_state, buf);
    draw_status_msg_bar(ed_state, buf);
    reset_cursor_pos(ed_state, buf);
    buf.append(esc_seq::SHOW_CURSOR);

    write(STDOUT_FILENO, buf.c_str(), buf.size());
}
