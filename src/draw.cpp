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
                ed_state.c_row() + 1, ed_state.c_col() + 1).c_str());
}

void refresh_screen(editor_state& ed_state)
{
    auto buf = str();

    buf.append(esc_char::HIDE_CURSOR);
    buf.append(esc_char::CLEAR_CURSOR_POS);
    draw_rows(ed_state, buf);
    reset_cursor_pos(ed_state, buf);
    buf.append(esc_char::SHOW_CURSOR);

    write(STDOUT_FILENO, buf.chars(), buf.len());
}
