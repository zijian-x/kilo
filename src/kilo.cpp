#include <cerrno>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include <fmt/core.h>

#define ESCAPE 27

struct termios orig_termios, raw;

void die(const char* s)
{
    perror(s);
    exit(1);
}

void disable_raw_mode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enable_raw_mode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");

    std::atexit(disable_raw_mode);

    raw = orig_termios;
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
                    // IXON flag: ctrl-s, ctrl-q
                    // ICRNL flag: ctrl-m
                    // BRKINT flag: passing `SIGINT` to the program
                    // INPCK flag: parity checking, not useful for modern
                    // termial emulator
                    // ISTRIP flag: stripping 8-bit input, probably already
                    // turned off
    raw.c_oflag &= ~(OPOST);
                    // OPOST flag: substituing every \n with \r automatically
    raw.c_cflag |= (CS8);
                    // CS8: set the character to 8 bits per byte, probably
                    // already set
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
                    // ECHO flag: not echoing out the entered char onto the
                    // terminal, like `sudo`
                    // ICANON flag: cooked mode
                    // ISIG flag: signals (ctrl-z, ctrl-c)
                    // IEXTEN flag: ctrl-v

    // for `read()`
    raw.c_cc[VMIN] = 0; // set minimum number of bytes of input needed to return
    raw.c_cc[VTIME] = 1; // set maximum amount of time to wait until return

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

int main(int argc, char** argv)
{
    enable_raw_mode();

    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) // EAGAIN errno
                                                                // for input
                                                                // timeout
            die("read");
        if (c == 'q')
            break;

        if (iscntrl(c)) {
            fmt::print("{:d}\r\n", c);
        } else {
            fmt::print("{:d} ('{:c}')\r\n", c, c);
        }
    }

    return 0;
}
