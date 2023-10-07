#include <exception>
#include <fmt/core.h>

#include "draw.hpp"
#include "exception_handler.hpp"
#include "file_io.hpp"
#include "read_input.hpp"
#include "termios_raii.hpp"

static editor_state ed_state;

int main(int argc, char** argv)
{
    t_ios.enable_raw_mode();
    std::set_terminate(exception_handler);

    if (argc >= 2)
        file::read_file(ed_state, argv[1]);

    while (1) {
        refresh_screen(ed_state);
        process_key_press(ed_state);
    }

    return 0;
}
