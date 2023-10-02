#pragma once

#include <cstddef>
#include <limits>
#include <utility>

class str
{
public:
    str() = default;

    str(const char*);

    str(char*&&);

    ~str()
    { delete[] m_str; }

    str(const str&);

    str(str&&);

    str& operator=(str);

    char& operator[](std::size_t i)
    { return m_str[i]; }

    const char& operator[](std::size_t i) const
    { return m_str[i]; }

    friend void swap(str&, str&);

    const char* chars() const
    { return this->m_str; }

    std::size_t len() const
    { return this->m_len; }

    char& front()
    { return m_str[0]; }

    char& front() const
    { return const_cast<str&>(*this).front(); }

    char& back()
    { return m_str[m_len - 1]; }

    char& back() const
    { return const_cast<str&>(*this).back(); }

    void push_back(char);

    str& append(const str&,
            std::size_t count = std::numeric_limits<std::size_t>::max());

    str& insert(std::size_t, std::size_t, char);

    str& insert(std::size_t, const str&);

    // TODO replace

    str& clear();

    str& remove_newline();

private:
    char* m_str{nullptr};
    std::size_t m_len{0};
    std::size_t m_size{0};

    void try_realloc_str(std::size_t extra_len);
};
