#include <algorithm>
#include <bits/chrono.h>
#include <cctype>
#include <chrono>
#include <format>
#include <functional>
#include <unistd.h>

#include "draw.hpp"
#include "editor_state.hpp"
#include "editor_keys.hpp"
#include "str.hpp"

static constexpr const char* KILO_VERS = "0.0.1";

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
    buf.append(char_seq::NEW_LINE);
}

str render_row(const str& row)
{
    auto render = str();

    for (size_t i = 0; i < row.size(); ++i) {
        if (row[i] == '\t')
            render.append(TABSTOP, ' ');
        else
            render.push_back(row[i]);
    }

    return render;
}


void draw_rows(editor_state& ed_state, str& buf)
{
    const auto& rows = ed_state.rows();
    for (size_t i = 0; i < ed_state.screen_row(); ++i) {
        if (auto file_row = i + ed_state.rowoff(); file_row < rows.size()) {
            const auto render = render_row(rows[file_row]);
            auto start_index = std::min(ed_state.coloff(), render.size());

            buf.append(render.c_str() + start_index, ed_state.screen_col());
        } else if (!rows.size() && i == ed_state.screen_row() >> 1) {
            print_welcome(ed_state, buf);
        } else {
            buf.push_back('~');
        }

        buf.append(esc_seq::CLEAR_LINE);
        buf.append(char_seq::NEW_LINE);
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
