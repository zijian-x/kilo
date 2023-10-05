#include <cstring>
#include <fcntl.h>
#include <fmt/format.h>
#include <unistd.h>

#include "file_io.hpp"
#include "editor_state.hpp"

namespace file
{
    void read_file(editor_state& ed_state, const char* filename)
    {
        auto fp = file_raii(filename);
        ed_state.filename() = fp.filename();

        for (auto line = fp.next_line(); line.len(); line = fp.next_line())
            ed_state.content().push_back(std::move(line.remove_newline()));
    }

    void save_file(editor_state& ed_state)
    {
        if (!ed_state.filename().len())
            return;

        const auto& buf = ed_state.rows_to_string();

        // TODO use file_raii, but first need to change file_raii ctor
        auto fd = open(ed_state.filename().chars(), O_RDWR | O_CREAT, 0644);
        if (fd != -1 && !ftruncate(fd, static_cast<long>(buf.len()))) {
            write(fd, buf.chars(), buf.len());
            ed_state.status_msg().set_msg(
                    fmt::format("{} bytes written to disk", buf.len()).c_str());
        } else {
            ed_state.status_msg().set_msg(
                    fmt::format("Can't save! I/O error: {}", std::strerror(errno)).c_str());

        }

        close(fd);
    }
}
