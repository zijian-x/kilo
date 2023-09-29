#pragma once

#include <utility>

class str
{
public:
    str() = default;
    str(const char* str);
    str(char*&& str);
    ~str()
    { delete[] m_str; }

    str(const str& buf);
    str(str&& buf);
    str& operator=(str buf);
    friend void swap(str& lhs, str& rhs);
    inline const char* chars() const { return this->m_str; }
    inline std::size_t len() const { return this->m_len; }
    inline char& front() { return m_str[0]; }
    inline char& front() const { return const_cast<str&>(*this).front(); }
    inline char& back() { return m_str[m_len - 1]; }
    inline char& back() const { return const_cast<str&>(*this).back(); }

    void push_back(char c);
    str& append(const char* str);
    str& clear();
    str& remove_newline();

private:
    char* m_str{};
    std::size_t m_len{};
    std::size_t m_size{};
};
