#include "draw.hpp"
#include "file_raii.hpp"
#include "read_input.hpp"
#include "termios_raii.hpp"
#include <iostream>

static editor_state ed_state;

void open_file(const char* filename)
{
    auto fp = file_raii(filename);
}

static void test()
{
}

int main(int argc, char** argv)
{
    static termios_raii t_ios;
    t_ios.enable_raw_mode();

    if (argc >= 2) {
        open_file(argv[1]);
    }

    while (1) {
        refresh_screen(ed_state);
        process_key_press(ed_state);
    }

    return 0;
}
