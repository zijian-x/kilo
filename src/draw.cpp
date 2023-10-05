#include <algorithm>
#include <bits/chrono.h>
#include <chrono>
#include <fmt/format.h>
#include <functional>

#include "draw.hpp"
#include "keycode.hpp"
#include "str.hpp"

static constexpr const char* KILO_VERS = "0.0.1";

void print_welcome(editor_state& ed_state, str& buf)
{
    auto msg = fmt::format("Kilo editor -- version {}", KILO_VERS);
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

    buf.append(esc_char::CLEAR_LINE);
    const auto& status_msg = ed_state.status_msg();
    if (status_msg.msg().len() &&
            duration_cast<seconds>(system_clock::now()
                - status_msg.timestamp()).count() < 2)
        buf.append(status_msg.msg(), ed_state.screen_col());
}

void draw_statusbar(editor_state& ed_state, str& buf)
{
    buf.append(esc_char::INVERT_COLOR);

    auto file_info = str(fmt::format("KILO_EDITOR | {} - {} lines{}",
                (ed_state.filename().len() ?
                 ed_state.filename().chars() : "[No Name]"),
                ed_state.content().size(),
                (ed_state.dirty() ? " [+]" : "")).c_str());
    auto line_info = str(fmt::format("{}:{}",
                ed_state.c_row() + 1, ed_state.c_col() + 1).c_str());

    file_info.resize(ed_state.screen_col() - line_info.len(), ' ');
    buf.append(std::move(file_info));
    buf.append(std::move(line_info));

    buf.append(esc_char::RESET_COLOR);
    buf.append(esc_char::NEWLINE);
}

void draw_rows(editor_state& ed_state, str& buf)
{
    const auto& content = ed_state.content();
    for (unsigned int i = 0; i < ed_state.screen_row(); ++i) {
        if (auto file_row = i + ed_state.rowoff(); file_row < content.size()) {
            const auto& line = content[file_row].render();
            auto start_index = std::min(static_cast<size_t>(ed_state.coloff()),
                    line.len());
            buf.append(line.chars() + start_index, ed_state.screen_col());
        } else if (!content.size() && i == ed_state.screen_row() >> 1) {
            print_welcome(ed_state, buf);
        } else {
            buf.push_back('~');
        }

        buf.append(esc_char::CLEAR_LINE);
        buf.append(esc_char::NEWLINE);
    }
}

void reset_cursor_pos(editor_state& ed_state, str& buf)
{
    buf.append(fmt::format("\x1b[{:d};{:d}H",
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
    if (c_row < ed_state.content().size())
        r_col = ed_state.content()[c_row].c_col_to_r_col(c_col);

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
    buf.append(esc_char::HIDE_CURSOR);
    buf.append(esc_char::CLEAR_CURSOR_POS);
    draw_rows(ed_state, buf);
    draw_statusbar(ed_state, buf);
    draw_status_msg_bar(ed_state, buf);
    reset_cursor_pos(ed_state, buf);
    buf.append(esc_char::SHOW_CURSOR);

    write(STDOUT_FILENO, buf.chars(), buf.len());
}
