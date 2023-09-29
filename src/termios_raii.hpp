#pragma once

#include <termios.h>
#include <unistd.h>

#include "die.hpp"

class termios_raii
{
public:
    termios_raii()
    {
        if (tcgetattr(STDIN_FILENO, &bak) == -1) { }
        raw = bak;
    }

    ~termios_raii()
    {
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &bak) == -1)
            die("tcsetattr");
    }

    void enable_raw_mode()
    {

        raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
        raw.c_oflag &= ~(OPOST);
        raw.c_cflag |= (CS8);
        raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 1;

        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
            die("tcsetattr");
    }

private:
    termios bak;
    termios raw;
};
