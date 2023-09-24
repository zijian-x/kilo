#pragma once

#include <optional>
#include <sys/ioctl.h>
#include <unistd.h>
#include <utility>

#include "utils.h"
#include "keycode.h"

class editor_state
{
public:
    unsigned int screen_row, screen_col;
    unsigned int c_row, c_col;

    editor_state() : c_row{0}, c_col{0}
    {
        set_win_size();
    }

    void move_curor(int c)
    {
        switch (c) {
            case editor_key::LEFT:
                if (c_col)
                    --c_col;
                break;
            case editor_key::DOWN:
                if (c_row < screen_row - 1)
                    ++c_row;
                break;
            case editor_key::UP:
                if (c_row)
                    --c_row;
                break;
            case editor_key::RIGHT:
                if (c_col < screen_col - 1)
                    ++c_col;
                break;
            case editor_key::PAGE_UP:
                c_row = 0; // FIXME move cursor to the top/bottom for now
                break;
            case editor_key::PAGE_DOWN:
                while (c_row < screen_row - 1) // FIXME
                    ++c_row;
            case editor_key::HOME:
                c_col = 0;
                break;
            case editor_key::END:
                c_col = screen_col - 1;
                break;
            case editor_key::DEL:
                // TODO
                break;
        }
    }

private:
    std::optional<std::pair<unsigned int, unsigned int>> get_win_size()
    {
        winsize ws;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_row == 0)
            return {};
        return std::make_pair(ws.ws_row, ws.ws_col);
    }

    void set_win_size()
    {
        auto ws = get_win_size();
        if (!ws.has_value())
            die("get_win_size");

        auto [row, col] = ws.value();
        this->screen_row = row;
        this->screen_col = col;
    }
};

