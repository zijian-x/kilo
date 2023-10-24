#include <cstring>
#include <fcntl.h>
#include <format>
#include <iomanip>
#include <unistd.h>

#include "file_io.hpp"
#include "editor.hpp"
#include "read_input.hpp"

namespace file
{
    void read_file(editor& ed, const char* filename)
    {
        auto fp = file_raii(filename);
        ed.filename() = fp.filename();

        for (auto line = fp.next_line(); line.size(); line = fp.next_line())
            ed.rows().emplace_back(std::move(line.remove_newline()));
        ed.set_ft();
    }

    void save_file(editor& ed)
    {
        if (ed.filename().empty()) {
            ed.filename() = prompt_input(ed, "Save as: ");
            if (ed.filename().empty()) {
                ed.status_msg().set_content("Saving aborted");
                return;
            }
            ed.set_ft();
        }

        const auto& buf = ed.rows_to_string();

        // TODO use file_raii, but first need to change file_raii ctor to be
        // able to accept O_* flags
        auto fd = open(ed.filename().c_str(), O_RDWR | O_CREAT, 0644);
        if (fd != -1 && !ftruncate(fd, static_cast<long>(buf.size()))) {
            write(fd, buf.c_str(), buf.size());
            ed.status_msg().set_content(
                    std::format("{} bytes written to disk", buf.size()).c_str());
            ed.dirty() = 0;
        } else {
            ed.status_msg().set_content(
                    std::format("Can't save! I/O error: {}", std::strerror(errno)).c_str());
        }

        close(fd);
    }
}
