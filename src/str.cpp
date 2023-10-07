#include "str.hpp"

#include <cctype>
#include <cstddef>
#include <cstring>
#include <limits>
#include <stdexcept>

str::str(const char* s)
{
    if (!s) [[unlikely]]
        return;

    m_size = std::strlen(s);
    m_capacity = m_size + 1;

    m_str = new char[m_capacity];
    std::strcpy(m_str, s);
}

str::str(char*&& s)
    : m_size{std::strlen(s)}
{
    m_capacity = m_size + 1;
    m_str = s;
}

str::str(const str& s)
    : m_size{s.m_size}
    , m_capacity{s.m_capacity}
{
    if (m_capacity) {
        m_str = new char[m_capacity]{};
        std::strcpy(m_str, s.m_str);
    }
}

str::str(str&& s)
    : m_str{std::exchange(s.m_str, nullptr)}
    , m_size{s.m_size}
    , m_capacity{s.m_capacity}
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
    swap(lhs.m_size, rhs.m_size);
    swap(lhs.m_capacity, rhs.m_capacity);
}

void str::push_back(char c)
{
    this->append(1, c);
}

str& str::append(std::size_t count, char c)
{
    try_realloc_str(m_size + count);
    std::memset(m_str + m_size, c, count);
    m_size += count;
    m_str[m_size] = 0;

    return *this;
}

str& str::append(const str& s, std::size_t n)
{
    auto copy_len = std::min(s.size(), n);
    if (!copy_len) [[unlikely]]
        return *this;

    try_realloc_str(m_size + copy_len + 1);
    std::memcpy(m_str + m_size, s.c_str(), copy_len);
    m_size += copy_len;
    m_str[m_size] = '\0';

    return *this;
}

str& str::resize(std::size_t count, char c)
{
    try_realloc_str(count);
    for (auto i = m_size; i < count; ++i)
        m_str[i] = c;
    m_size = count;
    m_str[m_size] = 0;

    return *this;
}

str& str::insert(std::size_t index, std::size_t count, int c)
{
    if (!count) [[unlikely]]
        return *this;
    if (index > m_size) [[unlikely]]
        std::out_of_range("accessing index beyond the underlying buffer size");

    try_realloc_str(m_size + count);
    // a b c d e f      idx = 2, count = 2 =>
                        // 1. arg = index + count = 2 + 2 = 4
                        // 2. arg = index = 2
                        // 3. arg = m_size - index + 1 = 6 - 2 + 1 = 5
    auto* dest = m_str + index + count;
    auto* src = m_str + index;
    auto n = m_size - index + 1;
    std::memmove(dest, src, n);
    for (size_t i = 0; i < count; ++i)
        m_str[index + i] = static_cast<char>(c);
    m_size += count;
    return *this;
}

str& str::insert(std::size_t index, const str& s)
{
    if (!s.m_size) [[unlikely]]
        return *this;
    if (index > m_size) [[unlikely]]
        std::out_of_range("erase index out of range");

    try_realloc_str(m_size + s.m_size);
    std::memmove(m_str + index + s.m_size, m_str + index, m_size - index + 1);
    std::memcpy(m_str + index, s.m_str, s.m_size);
    m_size += s.m_size;
    return *this;
}

str& str::clear()
{
    m_str[0] = 0;
    m_size = 0;
    return *this;
}

str& str::remove_newline()
{
    if (!m_size)
        return *this;

    while (m_str[m_size - 1] == '\r' || m_str[m_size - 1] == '\n')
        m_str[--m_size] = '\0';
    return *this;
}

void str::try_realloc_str(std::size_t new_len)
{
    if (new_len < m_capacity)
        return;

    m_capacity = new_len + 1;
    auto* new_str = new char[m_capacity]{};
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
    if (index > m_size) [[unlikely]]
        std::out_of_range("accessing index beyond the underlying buffer size");

    if (count == count2) {
        auto new_len = index + count;
        try_realloc_str(new_len);
        for (size_t i = 0; i < count; ++i)
            m_str[index + i] = c;
        m_size = std::max(m_size, new_len);
    } else if (count < count2) {
        // 0 1 2 3 4 5 6 7 8
        // a a a a a            idx = 2, cnt = 2, cnt2 = 4, char = b
        // a a b b b b a
        //  realloc = m_size - std::min(count, m_size - index) + count2 = 5 - 2 + 4 = 7
        auto new_len = m_size + count2 - std::min(count, m_size - index);
        try_realloc_str(new_len);

        //  std::memmove():
        //      1. arg dest: idx + cnt2 = 2 + 4 = 6
        //      2. arg src: idx + cnt = 2 + 2 = 4
        //      3. arg n: m_size - idx - cnt = 5 - 2 - 2 = 1
        auto* dest = m_str + index + count2;
        auto* src = m_str + index + count;
        auto n = m_size - index - count;
        m_size = new_len;
        if (dest < m_str + m_size)
            std::memmove(dest, src, n);
        for (size_t i = 0; i < count2; ++i)
            m_str[index + i] = c;
    } else {
        // 0 1 2 3 4 5 6 7
        // a a a a a        index = 1, count = 10, count2 = 5
        // a b b b b b
        auto replace = std::min(count, m_size - index);
        for (size_t i = 0; i < replace; ++i)
            m_str[index + i] = 0;

        if (replace < count2) {
            try_realloc_str(m_size + count2 - replace);
            m_size = m_size + count2 - replace;
        } else {
            // 0 1 2 3 4 5 6 7 8    len = 9
            // a a a a a c c c c    index = 1, count = 4, count2 = 2, char = b
            // a b b c c c c
            // std::memmove():
            //      1. arg dest: 3 = index + count2
            //      2. arg src: 5 = index + count
            //      3. arg n: 4 = m_size - index - count = 9 - 1 - 4
            auto* dest = m_str + index + count2;
            auto* src = m_str + index + replace;
            auto n = m_size - index - replace;
            if (dest < m_str + m_size)
                std::memmove(dest, src, n);
            m_size -= replace - count2;
        }

        for (size_t i = 0; i < count2; ++i)
            m_str[index + i] = c;
        m_str[m_size] = 0;
    }

    return *this;
}

str& str::erase(std::size_t index, std::size_t count)
{
    if (index > m_size)
        std::out_of_range("accessing index beyond the underlying buffer size");

    if (count == std::numeric_limits<std::size_t>::max()
            || index + count >= m_size) {
        m_str[index] = 0;
        m_size = index;
        return *this;
    }

    // 0 1 2 3 4
    // h e l l o
    //              index = 1, count 2
    //              dest = index = 1
    //              src = index + count = 1 + 2 = 3
    //              n = m_size - index - count = 5 - 1 - 2 = 2
    auto* dest = m_str + index;
    auto* src = m_str + index + count;
    auto n = m_size - index - count;
    std::memmove(dest, src, n);
    m_size -= count;
    m_str[m_size] = 0;

    return *this;
}
