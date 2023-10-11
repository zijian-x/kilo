#include "read_input.hpp"
#include "draw.hpp"
#include "editor_state.hpp"
#include "file_io.hpp"
#include "editor_keys.hpp"

#include <format>
#include <unistd.h>

static constexpr int ctrl_key(int c)
{ return c & 0x1f; }

static std::optional<int> read_arrow_key()
{
    char seq[3]{};
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
            throw std::runtime_error("error on read()");
    }

    return c == editor_key::ESCAPE ? read_arrow_key().value_or(c) : c;
}

str prompt_input(editor_state& ed_state, const str& prompt,
        std::optional<std::function<void(editor_state&, const str&)>> callback)
{
    auto input = str();

    for (;;) {
        auto msg = std::format("{}{}", prompt.c_str(), input.c_str());
        ed_state.status_msg().set_content(msg.c_str());
        refresh_screen(ed_state);

        int c = read_key();
        if (c == editor_key::DEL
                || c == ctrl_key('h')
                || c == editor_key::BACKSPACE) {
            if (!input.empty())
                input.pop_back();
        } else if (c == editor_key::ESCAPE) {
            ed_state.status_msg().clear();
            input.clear();
            return input;
        } else if (c == '\r') {
            return input;
        } else if (!std::iscntrl('c')) {
            input.push_back(static_cast<char>(c));
        }

        if (callback.has_value())
            callback.value()(ed_state, input);
    }
}

void process_key_press(editor_state& ed_state)
{
    static unsigned int quit_times = QUIT_TIMES;
    auto& c_row = ed_state.c_row();
    auto& c_col = ed_state.c_col();
    const auto& rows = ed_state.rows();

    auto c = read_key();
    switch (c) {
        case '\r':
            ed_state.insert_newline();
            break;
        case ctrl_key('q'):
            if (ed_state.dirty() && quit_times) {
                --quit_times;
                ed_state.status_msg()
                    .set_content("File has unsaved changes, press again to quit");
                return;
            }
            quit_editor();
            break;
        case ctrl_key('s'):
            file::save_file(ed_state);
            break;
        case ctrl_key('f'):
            ed_state.find();
            break;
        case editor_key::BACKSPACE:
        case ctrl_key('h'):
        case editor_key::DEL:
            if (c == editor_key::DEL)
                ed_state.move_curor(editor_key::RIGHT);
            ed_state.delete_char();
            break;
        case ctrl_key('l'):
        case editor_key::ESCAPE:
            // ignore refresh and escape key
            break;
        case editor_key::PAGE_UP:
            c_row = ed_state.rowoff();
            for (size_t i = ed_state.screen_row(); i > 0; --i)
                ed_state.move_curor(editor_key::UP);
            break;
        case editor_key::PAGE_DOWN:
            c_row = std::min(rows.size() - 1,
                    ed_state.rowoff() + ed_state.screen_row() - 1);
            for (size_t i = ed_state.screen_row(); i > 0; --i)
                ed_state.move_curor(editor_key::DOWN);
            break;
        case editor_key::HOME:
            c_col = 0;
            break;
        case editor_key::END:
            if (c_row < rows.size()
                    && rows[c_row].size())
                c_col = rows[c_row].size();
            break;
        case editor_key::UP:
        case editor_key::DOWN:
        case editor_key::RIGHT:
        case editor_key::LEFT:
            ed_state.move_curor(c);
            break;
        default:
            ed_state.insert_char(c);
            break;
    }

    quit_times = QUIT_TIMES;
}

