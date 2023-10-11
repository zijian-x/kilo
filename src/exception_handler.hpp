#pragma once

#include <exception>
#include <unistd.h>

#include "termios_raii.hpp"
#include "editor_keys.hpp"

[[noreturn]] static inline void exception_handler()
{
    t_ios.disable_raw_mode();
    write(STDOUT_FILENO, esc_seq::CLEAR_SCREEN, 4);
    write(STDOUT_FILENO, esc_seq::CLEAR_CURSOR_POS, 3);

    std::exit(EXIT_FAILURE);
}
