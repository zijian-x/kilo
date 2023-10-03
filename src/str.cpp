#include "str.hpp"
#include "die.hpp"
#include <cctype>
#include <cstddef>
#include <cstring>
#include <fmt/core.h>
#include <limits>

str::str(const char* s)
{
    if (!s) [[unlikely]]
        return;

    m_len = std::strlen(s);
    m_size = m_len + 1;

    m_str = new char[m_size];
    std::strcpy(m_str, s);
}

str::str(char*&& s)
    : m_len{std::strlen(s)}
{
    m_size = m_len + 1;
    m_str = s;
}

str::str(const str& s)
    : m_len{s.m_len}
    , m_size{s.m_size}
{
    if (m_size) {
        m_str = new char[m_size]{};
        std::strcpy(m_str, s.m_str);
    }
}

str::str(str&& s)
    : m_str{std::exchange(s.m_str, nullptr)}
    , m_len{s.m_len}
    , m_size{s.m_size}
{ }

str& str::operator=(str s)
{
    swap(*this, s);
    return *this;
}

void swap(str& lhs, str& rhs)
{
    using std::swap;
    swap(lhs.m_str, rhs.m_str);
    swap(lhs.m_len, rhs.m_len);
    swap(lhs.m_size, rhs.m_size);
}

void str::push_back(char c)
{
    try_realloc_str(m_len + 1);
    m_str[m_len++] = c;
    m_str[m_len] = 0;
}

str& str::append(const str& s, std::size_t n)
{
    auto copy_len = std::min(std::strlen(s.chars()), n);
    if (!copy_len) [[unlikely]]
        return *this;

    try_realloc_str(m_len + copy_len + 1);
    std::memcpy(m_str + m_len, s.chars(), copy_len);
    m_len += copy_len;
    m_str[m_len] = '\0';

    return *this;
}

str& str::insert(std::size_t index, std::size_t count, char c)
{
    if (!count) [[unlikely]]
        return *this;
    if (index > m_len) [[unlikely]]
        die("insert index out of range");

    try_realloc_str(m_len + count);
    // a b c d e f      idx = 2, count = 2 =>
                        // 1. arg = index + count = 2 + 2 = 4
                        // 2. arg = index = 2
                        // 3. arg = m_len - index + 1 = 6 - 2 + 1 = 5
    auto* dest = m_str + index + count;
    auto* src = m_str + index;
    auto n = m_len - index + 1;
    std::memmove(dest, src, n);
    for (size_t i = 0; i < count; ++i)
        m_str[index + i] = c;
    m_len += count;
    return *this;
}

str& str::insert(std::size_t index, const str& s)
{
    if (!s.m_len) [[unlikely]]
        return *this;
    if (index > m_len) [[unlikely]]
        die("insert index out of range");

    try_realloc_str(m_len + s.m_len);
    std::memmove(m_str + index + s.m_len, m_str + index, m_len - index + 1);
    std::memcpy(m_str + index, s.m_str, s.m_len);
    m_len += s.m_len;
    return *this;
}

str& str::clear()
{
    m_str[0] = 0;
    m_len = 0;
    return *this;
}

str& str::remove_newline()
{
    if (!m_len)
        return *this;

    while (m_str[m_len - 1] == '\r' || m_str[m_len - 1] == '\n')
        m_str[--m_len] = '\0';
    return *this;
}

void str::try_realloc_str(std::size_t new_len)
{
    if (new_len < m_size)
        return;

    m_size = new_len + 1;
    auto* new_str = new char[m_size]{};
    if (!new_str)
        die("new[] alloc");

    if (m_str) {
        std::strcpy(new_str, m_str);
        delete[] m_str;
    }
    m_str = new_str;
}

str& str::replace(std::size_t index, std::size_t count,
        std::size_t count2, char c)
{
    if (!count || !count2) [[unlikely]]
        return *this;
    if (index > m_len) [[unlikely]]
        die("index out of bound");

    if (count == count2) {
        auto new_len = index + count;
        try_realloc_str(new_len);
        for (size_t i = 0; i < count; ++i)
            m_str[index + i] = c;
        m_len = std::max(m_len, new_len);
    } else if (count < count2) {
        // 0 1 2 3 4 5 6 7 8
        // a a a a a            idx = 2, cnt = 2, cnt2 = 4, char = b
        // a a b b b b a
        //  realloc = m_len - std::min(count, m_len - index) + count2 = 5 - 2 + 4 = 7
        auto new_len = m_len + count2 - std::min(count, m_len - index);
        try_realloc_str(new_len);

        //  std::memmove():
        //      1. arg dest: idx + cnt2 = 2 + 4 = 6
        //      2. arg src: idx + cnt = 2 + 2 = 4
        //      3. arg n: m_len - idx - cnt = 5 - 2 - 2 = 1
        auto* dest = m_str + index + count2;
        auto* src = m_str + index + count;
        auto n = m_len - index - count;
        m_len = new_len;
        if (dest < m_str + m_len)
            std::memmove(dest, src, n);
        for (size_t i = 0; i < count2; ++i)
            m_str[index + i] = c;
    } else {
        // 0 1 2 3 4 5 6 7
        // a a a a a        index = 1, count = 10, count2 = 5
        // a b b b b b
        auto replace = std::min(count, m_len - index);
        for (size_t i = 0; i < replace; ++i)
            m_str[index + i] = 0;

        if (replace < count2) {
            try_realloc_str(m_len + count2 - replace);
            m_len = m_len + count2 - replace;
        } else {
            // 0 1 2 3 4 5 6 7 8    len = 9
            // a a a a a c c c c    index = 1, count = 4, count2 = 2, char = b
            // a b b c c c c
            // std::memmove():
            //      1. arg dest: 3 = index + count2
            //      2. arg src: 5 = index + count
            //      3. arg n: 4 = m_len - index - count = 9 - 1 - 4
            auto* dest = m_str + index + count2;
            auto* src = m_str + index + replace;
            auto n = m_len - index - replace;
            if (dest < m_str + m_len)
                std::memmove(dest, src, n);
            m_len -= replace - count2;
        }

        for (size_t i = 0; i < count2; ++i)
            m_str[index + i] = c;
        m_str[m_len] = 0;
    }

    return *this;
}
