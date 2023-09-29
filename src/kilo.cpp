#include "draw.hpp"
#include "file_raii.hpp"
#include "read_input.hpp"
#include "termios_raii.hpp"
#include "str.hpp"
#include <fmt/core.h>

static editor_state ed_state;

void open_file(const char* filename)
{
    using namespace file;

    auto fp = file_raii(filename);
    char* buf = nullptr;
    while ((buf = fp.nextline()) != nullptr || !fp.eof()) {
        auto line = str(std::move(buf)).remove_newline();
        ed_state.content().push_back(std::move(line));
    }
}

static void test()
{
}

int main(int argc, char** argv)
{
    // test(); return 0;

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
