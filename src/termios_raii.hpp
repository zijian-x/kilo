#pragma once

#include <stdexcept>
#include <termios.h>
#include <unistd.h>

class termios_raii
{
public:
    termios_raii()
    {
        if (tcgetattr(STDIN_FILENO, &bak) == -1)
            throw std::runtime_error("termios error");
        raw = bak;
    }

    ~termios_raii()
    {
        disable_raw_mode();
    }

    void enable_raw_mode()
    {
        raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
        raw.c_oflag &= ~(OPOST);
        raw.c_cflag |= (CS8);
        raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
        raw.c_cc[VMIN] = 0; // TODO change to 1 maybe
        raw.c_cc[VTIME] = 1; // TODO change to 1 maybe

        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
            throw std::runtime_error("termios error");
    }

    void disable_raw_mode()
    { tcsetattr(STDIN_FILENO, TCSAFLUSH, &bak); }

private:
    termios bak;
    termios raw;
};

extern termios_raii t_ios;
