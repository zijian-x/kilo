#include <fmt/core.h>

#include "draw.hpp"
#include "file_raii.hpp"
#include "read_input.hpp"
#include "strbuf.hpp"
#include "termios_raii.hpp"

static editor_state ed_state;

void open_file(const char* filename)
{
    auto fp = file_raii(filename, "r");

    char* line = nullptr;
    // TODO read file
}

static void test()
{
}

int main(int argc, char** argv)
{
    static termios_raii t_ios;
    t_ios.enable_raw_mode();

    while (1) {
        refresh_screen(ed_state);
        process_key_press(ed_state);
    }

    return 0;
}
