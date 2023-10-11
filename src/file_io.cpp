#include <cstring>
#include <fcntl.h>
#include <format>
#include <unistd.h>

#include "file_io.hpp"
#include "editor_state.hpp"
#include "read_input.hpp"

namespace file
{
    void read_file(editor_state& ed_state, const char* filename)
    {
        auto fp = file_raii(filename);
        ed_state.filename() = fp.filename();

        for (auto line = fp.next_line(); line.size(); line = fp.next_line())
            ed_state.rows().push_back(std::move(line.remove_newline()));
    }

    void save_file(editor_state& ed_state)
    {
        if (ed_state.filename().empty()) {
            ed_state.filename() = prompt_input(ed_state, "Save as: ");
            if (ed_state.filename().empty()) {
                ed_state.status_msg().set_content("Saving aborted");
                return;
            }
        }

        const auto& buf = ed_state.rows_to_string();

        // TODO use file_raii, but first need to change file_raii ctor to be
        // able to accept O_* flags
        auto fd = open(ed_state.filename().c_str(), O_RDWR | O_CREAT, 0644);
        if (fd != -1 && !ftruncate(fd, static_cast<long>(buf.size()))) {
            write(fd, buf.c_str(), buf.size());
            ed_state.status_msg().set_content(
                    std::format("{} bytes written to disk", buf.size()).c_str());
            ed_state.dirty() = 0;
        } else {
            ed_state.status_msg().set_content(
                    std::format("Can't save! I/O error: {}", std::strerror(errno)).c_str());
        }

        close(fd);
    }
}
