#pragma once

#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "keycode.h"

static inline void die(const char* s)
{
    std::perror(s);
    std::exit(EXIT_FAILURE);

    write(STDOUT_FILENO, esc_char::CLEAR_SCREEN, 4);
    write(STDOUT_FILENO, esc_char::CLEAR_CURSOR_POS, 3);
}
