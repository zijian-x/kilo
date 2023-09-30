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

    delete[] m_str;
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
    m_str = new char[m_size]{};
    std::strcpy(m_str, s.m_str);
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
    if (!copy_len)
        return *this;

    try_realloc_str(m_len + copy_len + 1);
    std::memcpy(m_str + m_len, s.chars(), copy_len);
    m_len += copy_len;
    m_str[m_len] = '\0';

    return *this;
}

str& str::insert(std::size_t index, std::size_t count, char c)
{
    if (!count)
        return *this;
    if (index > m_len)
        die("insert index out of range");

    try_realloc_str(m_len + count);
    // a b c d e f      idx = 2, count = 2 =>
                        // 1. arg = index + count = 2 + 2 = 4
                        // 2. arg = index = 2
                        // 3. arg = m_len - index + 1 = 6 - 2 + 1 = 5
    std::memmove(m_str + index + count, m_str + index, m_len - index + 1);
    for (size_t i = 0; i < count; ++i)
        m_str[index + i] = c;
    return *this;
}

str& str::insert(std::size_t index, const str& s)
{
    if (!s.m_len)
        return *this;
    if (index > m_len)
        die("insert index out of range");

    try_realloc_str(m_len + s.m_len);
    std::memmove(m_str + index + s.m_len, m_str + index, m_len - index + 1);
    std::memcpy(m_str + index, s.m_str, s.m_len);
    return *this;
}

str& str::clear()
{
    delete[] m_str;
    m_len = m_size = 0;
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
    auto* new_str = new char[m_size];
    if (!new_str)
        die("new[] alloc");

    std::strcpy(new_str, m_str);
    delete[] m_str;
    m_str = new_str;
}
