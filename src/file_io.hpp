#pragma once

#include <stdexcept>
#include <cstring>
#include <stdio.h>

#include "editor_state.hpp"
#include "str.hpp"

namespace file
{
    class file_raii
    {
    public:
        file_raii(const char* filename) : file_raii(filename, "r")
        { }

        file_raii(const char* filename, const char* mode)
        {
            this->m_filename = filename;
            this->m_fp = fopen(filename, mode);
            if (!m_fp)
                throw std::invalid_argument("file not found");
        }

        file_raii(const file_raii&) = delete;
        file_raii(file_raii&&) = delete;

        ~file_raii()
        { fclose(m_fp); }

        file_raii& operator=(file_raii) = delete;

        const str& filename() const
        { return this->m_filename; }

        FILE* fp() { return this->m_fp; }
        const FILE* fp() const { return this->m_fp; }

        bool eof()
        { return feof(m_fp); }

        str next_line()
        {
            auto line = str();
            char* buf{};
            size_t size{};
            ssize_t len{};

            if ((len = getline(&buf, &size, m_fp)) > 0)
                line = buf;
            free(buf);

            return line;
        }

        void write_line(const str& line)
        {
            fwrite(line.c_str(), sizeof(char), line.size(), m_fp);
            if (line.back() != '\r' && line.back() != '\n')
                fwrite("\n", sizeof(char), 1, m_fp);
        }

    private:
        str m_filename;
        FILE* m_fp;
    };

    void read_file(editor_state& ed_state, const char* filename);

    void save_file(editor_state& ed_state);
}
