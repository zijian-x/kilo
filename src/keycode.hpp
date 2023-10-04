#pragma once

#include <array>
#include <algorithm>
#include <stdexcept>

namespace esc_char
{
    static constexpr const char* NEWLINE = "\r\n";
    static constexpr const char* CLEAR_SCREEN = "\x1b[2J";
    static constexpr const char* CLEAR_LINE = "\x1b[K";
    static constexpr const char* CLEAR_CURSOR_POS = "\x1b[H";
    static constexpr const char* HIDE_CURSOR = "\x1b[?25l";
    static constexpr const char* SHOW_CURSOR = "\x1b[?25h";
    static constexpr const char* INVERT_COLOR = "\x1b[7m";
    static constexpr const char* RESET_COLOR = "\x1b[m";
}

enum editor_key : int
{
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

static constexpr inline auto lookup = std::array<editor_key, 4>{
    editor_key::UP, editor_key::DOWN, editor_key::LEFT, editor_key::RIGHT
};

static constexpr editor_key get_key(char c)
{
    auto e = std::find(begin(lookup), end(lookup), static_cast<editor_key>(c));
    if (e == end(lookup))
        throw std::invalid_argument("invalid editor key");
    return *e;
}
