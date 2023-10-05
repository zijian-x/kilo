#include "draw.hpp"
#include "file_io.hpp"
#include "read_input.hpp"
#include "termios_raii.hpp"
#include <fmt/core.h>

static editor_state ed_state;

int main(int argc, char** argv)
{
    static termios_raii t_ios;
    t_ios.enable_raw_mode();

    if (argc >= 2) {
        file::read_file(ed_state, argv[1]);
    }

    while (1) {
        refresh_screen(ed_state);
        process_key_press(ed_state);
    }

    return 0;
}
