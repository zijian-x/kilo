#include <cerrno>
#include <stdlib.h>
#include <string>
#include <termios.h>
#include <unistd.h>

#include <fmt/format.h>

#include "editor_state.h"
#include "keycode.h"
#include "termios_raii.h"
#include "utils.h"

using abuf = std::string;

static constexpr const char* KILO_VERS = "0.0.1";

static editor_state ed_state;

std::optional<int> read_arrow_key()
{
    char seq[3]{}; // will need size of 3 in da future
    if (read(STDIN_FILENO, &seq[0], 1) != 1)
        return {};
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
        return {};
    if (seq[0] == '[') {
        if (seq[1] >= '0' && seq[1] <= '9') {
            if (read(STDIN_FILENO, &seq[2], 1) != 1)
                return {};
            if (seq[2] == '~') {
                switch (seq[1]) {
                    case '1': return editor_key::HOME;
                    case '3': return editor_key::DEL;
                    case '4': return editor_key::END;
                    case '5': return editor_key::PAGE_UP;
                    case '6': return editor_key::PAGE_DOWN;
                    case '7': return editor_key::HOME;
                    case '8': return editor_key::END;
                }
            }
        } else {
            switch (seq[1]) {
                case 'A': return editor_key::UP;
                case 'B': return editor_key::DOWN;
                case 'C': return editor_key::RIGHT;
                case 'D': return editor_key::LEFT;
                case 'H': return editor_key::HOME;
                case 'F': return editor_key::END;
            }
        }
    } else if (seq[0] == '0') {
        switch (seq[1]) {
            case 'H': return editor_key::HOME;
            case 'F': return editor_key::END;
        }
    }
    return {};
}

int read_key()
{
    ssize_t nread;
    int c = '\0';
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN)
            die("read");
    }

    return c == '\x1b' ? read_arrow_key().value_or(c) : c;
}

void process_key_press()
{
    auto c = read_key();
    switch (c) {
        case ctrl_key('q'):
            write(STDOUT_FILENO, esc_char::CLEAR_SCREEN, 4);
            write(STDOUT_FILENO, esc_char::CLEAR_CURSOR_POS, 3);
            std::exit(0);
            break;
        case editor_key::UP:
        case editor_key::DOWN:
        case editor_key::RIGHT:
        case editor_key::LEFT:
        case editor_key::PAGE_UP:
        case editor_key::PAGE_DOWN:
        case editor_key::HOME:
        case editor_key::END:
        case editor_key::DEL:
            ed_state.move_curor(c);
            break;
    }
}

void print_welcome(abuf& buf)
{
    auto msg = fmt::format("Kilo editor -- version {}", KILO_VERS);
    if (msg.size() > ed_state.screen_col)
        return;

    auto padding = (ed_state.screen_col - msg.size() - 1) / 2;
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
    for (unsigned int i = 0; i < ed_state.screen_row; ++i) {
        if (i == ed_state.screen_row / 3) {
            print_welcome(buf);
        } else {
            buf.push_back('~');
        }

        buf.append(esc_char::CLEAR_LINE);
        if (i < ed_state.screen_row - 1)
            buf.append("\r\n");
    }
}

void reset_cursor_pos(abuf& buf)
{
    buf.append(fmt::format("\x1b[{:d};{:d}H",
                ed_state.c_row + 1, ed_state.c_col + 1));
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
        process_key_press();
    }

    return 0;
}
