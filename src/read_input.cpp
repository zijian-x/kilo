#include "read_input.hpp"
#include "die.hpp"
#include "keycode.hpp"

static std::optional<int> read_arrow_key()
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

static int read_key()
{
    ssize_t nread;
    int c = '\0';
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN)
            die("read");
    }

    return c == '\x1b' ? read_arrow_key().value_or(c) : c;
}

static constexpr int ctrl_key(int c)
{
    return c & 0x1f;
}

void process_key_press(editor_state& ed_state)
{
    auto& c_row = ed_state.c_row();
    auto& c_col = ed_state.c_col();
    const auto& contents = ed_state.content();

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
            ed_state.move_curor(c);
            break;
        case editor_key::PAGE_UP:
            c_row = ed_state.rowoff();
            for (size_t i = ed_state.screen_row(); i > 0; --i)
                ed_state.move_curor(editor_key::UP);
            break;
        case editor_key::PAGE_DOWN:
            c_row = std::min(contents.size() - 1,
                    ed_state.rowoff() + ed_state.screen_row() - 1);
            for (size_t i = ed_state.screen_row(); i > 0; --i)
                ed_state.move_curor(editor_key::DOWN);
            break;
        case editor_key::HOME:
            c_col = 0;
            break;
        case editor_key::END:
            if (c_row < contents.size()
                    && contents[c_row].content().len())
                c_col = contents[c_row].content().len();
            break;
    }
}

