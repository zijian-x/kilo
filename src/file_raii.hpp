#pragma once

#include <cstring>
#include <fmt/core.h>
#include <stdio.h>

#include "utils.hpp"

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

private:
    FILE* m_fp;
};
