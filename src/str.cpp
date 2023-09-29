#include "str.hpp"
#include "utils.hpp"
#include <cctype>
#include <cstring>

str::str(const char* str)
{
    if (str) {
        m_len = std::strlen(str);
        m_size = m_len + 1;
        m_str = new char[m_size];
        std::strcpy(m_str, str);
    }
}

str::str(char*&& str)
    : m_len{std::strlen(str)}
{
    m_size = m_len + 1;
    m_str = str;
}

str::str(const str& buf)
    : m_len{buf.m_len}
    , m_size{buf.m_size}
{
    m_str = new char[m_size]{};
    std::strcpy(m_str, buf.m_str);
}

str::str(str&& buf)
    : m_str{std::exchange(buf.m_str, nullptr)}
    , m_len{buf.m_len}
    , m_size{buf.m_size}
{ }

str& str::operator=(str buf)
{
    swap(*this, buf);
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

str& str::append(const char* str)
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
