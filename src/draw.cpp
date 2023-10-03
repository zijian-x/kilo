#include <fmt/format.h>

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

void draw_rows(editor_state& ed_state, str& buf)
{
    for (unsigned int i = 0; i < ed_state.screen_row(); ++i) {
        const auto& lines = ed_state.content();

        if (auto file_row = i + ed_state.rowoff(); file_row < lines.size()) {
            const auto& printline = lines[file_row].render();
            auto start_index = std::min(static_cast<size_t>(ed_state.coloff()),
                    printline.len());
            buf.append(printline.chars() + start_index, ed_state.screen_col());
        } else if (!lines.size() && i == ed_state.screen_row() >> 1) {
            print_welcome(ed_state, buf);
        } else {
            buf.push_back('~');
        }

        buf.append(esc_char::CLEAR_LINE);
        if (i < ed_state.screen_row() - 1)
            buf.append("\r\n");
    }
}

void reset_cursor_pos(editor_state& ed_state, str& buf)
{
    buf.append(fmt::format("\x1b[{:d};{:d}H",
                ed_state.c_row() + 1, ed_state.r_col() + 1).c_str());
}

void scroll(editor_state& ed_state)
{
    auto& screen_row = ed_state.screen_row();
    auto& screen_col = ed_state.screen_col();
    auto& c_row = ed_state.c_row();
    auto& c_col = ed_state.c_col();
    auto& r_col = ed_state.r_col();
    auto& rowoff = ed_state.rowoff();
    auto& coloff = ed_state.coloff();

    r_col = 0;
    if (c_row < screen_row)
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
    reset_cursor_pos(ed_state, buf);
    buf.append(esc_char::SHOW_CURSOR);

    write(STDOUT_FILENO, buf.chars(), buf.len());
}
