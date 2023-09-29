#pragma once

#include <cstring>
#include <stdio.h>

#include "str.hpp"
#include "utils.hpp"

namespace file
{
    class file_raii
    {
    public:
        file_raii(const char* filename) : file_raii(filename, "r") { }

        file_raii(const char* filename, const char* mode)
        {
            this->m_fp = fopen(filename, mode);
            if (!m_fp)
                die("fopen");
        }

        ~file_raii()
        {
            fclose(m_fp);
        }

        FILE* fp() { return this->m_fp; }
        const FILE* fp() const { return this->m_fp; }

        bool eof()
        {
            return feof(m_fp);
        }

        char* nextline()
        {
            char* buf{}, * str{};
            size_t size{};
            ssize_t len{};
            if ((len = getline(&buf, &size, m_fp)) > 0) {
                str = new char[static_cast<size_t>(len) + 1];
                if (!str)
                    die("new alloc");
                std::memcpy(str, buf, static_cast<size_t>(len));
                str[len] = '\0';
            }

            free(buf);
            return str;
        }

        void writeline(const str& line)
        {
            fwrite(line.chars(), sizeof(char), line.len(), m_fp);
            if (line.back() != '\r' || line.back() != '\n')
                fwrite("\n", sizeof(char), 1, m_fp);
        }

    private:
        FILE* m_fp;
    };
}
