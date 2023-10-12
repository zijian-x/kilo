#pragma once

#include <array>
#include <algorithm>
#include <stdexcept>

namespace char_seq
{
    static constexpr const char* NEW_LINE = "\r\n";

    namespace esc_seq
    {
        static constexpr const char* CLEAR_SCREEN = "\x1b[2J";
        static constexpr const char* CLEAR_LINE = "\x1b[K";
        static constexpr const char* CLEAR_CURSOR_POS = "\x1b[H";
        static constexpr const char* HIDE_CURSOR = "\x1b[?25l";
        static constexpr const char* SHOW_CURSOR = "\x1b[?25h";
        static constexpr const char* INVERT_COLOR = "\x1b[7m";
        static constexpr const char* RESET_COLOR = "\x1b[m";
    }

}

enum editor_key : int
{
    ESCAPE = '\x1b',
    BACKSPACE = 127,
    UP = 1000,
    DOWN,
    LEFT,
    RIGHT,
    PAGE_UP,
    PAGE_DOWN,
    HOME,
    END,
    DEL,
};

enum colors : int
{
    BLACK = 30,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    DEFAULT = 39
};
