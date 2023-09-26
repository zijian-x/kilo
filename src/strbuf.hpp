#pragma once

#include "utils.hpp"
#include <cstddef>
#include <cstring>
#include <fmt/core.h>
#include <iostream>
#include <utility>

class strbuf
{
public:
    strbuf()
    {}

    strbuf(const char* str) : m_len{std::strlen(str)}
    {
        m_size = m_len + 1;
        m_str = new char[m_size];
        std::strcpy(m_str, str);
    }

    strbuf(char*&& str) : m_len{std::strlen(str)}
    {
        m_size = m_len + 1;
        m_str = str;
    }

    ~strbuf()
    { delete[] m_str; }

    strbuf(const strbuf& buf)
        : m_len{buf.m_len}
        , m_size{buf.m_size}
    {
        m_str = new char[m_size]{};
        std::strcpy(m_str, buf.m_str);
    }

    strbuf(strbuf&& buf)
        : m_str{std::exchange(buf.m_str, nullptr)}
        , m_len{buf.m_len}
        , m_size{buf.m_size}
    { }

    strbuf& operator=(strbuf buf)
    {
        swap(*this, buf);
        return *this;
    }

    friend void swap(strbuf& lhs, strbuf& rhs)
    {
        using std::swap;
        swap(lhs.m_str, rhs.m_str);
        swap(lhs.m_len, rhs.m_len);
        swap(lhs.m_size, rhs.m_size);
    }

    const char* chars() const { return this->m_str; }
    size_t len() const { return this->m_len; }

    void push_back(char c)
    {
        if (!m_size) {
            m_size = 2;
            m_str = new char[m_size];
        } else if (m_len + 1 == m_size) {
            m_size <<= 1;
            auto* new_str = new char[m_size];
            if (!new_str)
                die("new[] alloc");

            std::memcpy(new_str, m_str, m_len);
            delete[] m_str;
            m_str = new_str;
        }

        m_str[m_len++] = c;
        m_str[m_len] = 0;
    }

    void append(const char* str)
    {
        auto strlen = std::strlen(str);
        if (!m_size) {
            m_size = strlen + 1;
        } else {
            while (m_size < m_len + strlen + 1) {
                m_size <<= 1;
            }
        }

        auto* new_str = new char[m_size];
        if (!new_str)
            die("new[] alloc");

        std::memcpy(new_str, m_str, m_len);
        std::strcpy(new_str + m_len, str);
        delete[] m_str;
        m_str = new_str;
        m_len += strlen;
    }

private:
    char* m_str{};
    size_t m_len{};
    size_t m_size{};
};
