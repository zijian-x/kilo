#pragma once

#include <cstdio>

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

private:
    FILE* m_fp;
};
