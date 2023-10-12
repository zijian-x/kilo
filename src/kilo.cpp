#include <exception>

#include "draw.hpp"
#include "exception_handler.hpp"
#include "file_io.hpp"
#include "read_input.hpp"
#include "termios_raii.hpp"

static editor ed;

int main(int argc, char** argv)
{
    t_ios.enable_raw_mode();
    std::set_terminate(exception_handler);

    if (argc >= 2)
        file::read_file(ed, argv[1]);

    for (;;) {
        refresh_screen(ed);
        process_key_press(ed);
    }

    return 0;
}
