#include <cerrno>
#include <stdlib.h>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <optional>

#include <fmt/format.h>

#include "read_input.hpp"
#include "termios_raii.hpp"

using abuf = std::string;

static constexpr const char* KILO_VERS = "0.0.1";

static editor_state ed_state;

void print_welcome(abuf& buf)
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
    buf.append(msg);
}

void draw_rows(abuf& buf)
{
    for (unsigned int i = 0; i < ed_state.screen_row(); ++i) {
        if (i == ed_state.screen_row() / 3) {
            print_welcome(buf);
        } else {
            buf.push_back('~');
        }

        buf.append(esc_char::CLEAR_LINE);
        if (i < ed_state.screen_row() - 1)
            buf.append("\r\n");
    }
}

void reset_cursor_pos(abuf& buf)
{
    buf.append(fmt::format("\x1b[{:d};{:d}H",
                ed_state.c_row() + 1, ed_state.c_col() + 1));
}

void refresh_screen()
{
    auto buf = abuf();

    buf.append(esc_char::HIDE_CURSOR);
    buf.append(esc_char::CLEAR_CURSOR_POS);
    draw_rows(buf);
    reset_cursor_pos(buf);
    buf.append(esc_char::SHOW_CURSOR);

    write(STDOUT_FILENO, buf.c_str(), buf.size());
}

int main(int argc, char** argv)
{
    static termios_raii t_ios;
    t_ios.enable_raw_mode();

    while (1) {
        refresh_screen();
        process_key_press(ed_state);
    }

    return 0;
}
