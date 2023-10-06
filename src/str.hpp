#pragma once

#include <bits/types/struct_itimerspec.h>
#include <cstddef>
#include <iterator>
#include <limits>
#include <utility>

class str
{
public:

    class iterator
    {
    public:
        using iterator_category = std::contiguous_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = char;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;

        iterator(pointer ptr) : m_ptr{ptr}
        {}

        reference operator*()
        { return *m_ptr; }

        const_reference operator*() const
        { return *m_ptr; }

        pointer operator->()
        { return m_ptr; }

        const_pointer operator->() const
        { return m_ptr; }

        iterator& operator++()
        {
            ++m_ptr;
            return *this;
        }

        iterator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator& operator--()
        {
            --m_ptr;
            return *this;
        }

        iterator operator--(int)
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        iterator operator+(difference_type diff)
        {
            auto tmp = *this;
            tmp += diff;
            return tmp;
        }

        iterator& operator+=(difference_type diff)
        {
            m_ptr += diff;
            return *this;
        }

        iterator operator-(difference_type diff)
        {
            auto tmp = *this;
            tmp -= diff;
            return tmp;
        }

        iterator& operator-=(difference_type diff)
        {
            m_ptr -= diff;
            return *this;
        }

        friend bool operator==(const iterator& lhs, const iterator& rhs)
        {
            return lhs.m_ptr == rhs.m_ptr;
        }

        friend bool operator<(const iterator& lhs, const iterator& rhs)
        {
            return lhs.m_ptr < rhs.m_ptr;
        }

        friend bool operator!=(const iterator& lhs, const iterator& rhs)
        {
            return !(lhs == rhs);
        }


        friend bool operator<=(const iterator& lhs, const iterator& rhs)
        {
            return lhs == rhs || lhs < rhs;
        }

        friend bool operator>(const iterator& lhs, const iterator& rhs)
        {
            return !(lhs <= rhs);
        }

        friend bool operator>=(const iterator& lhs, const iterator& rhs)
        {
            return !(lhs < rhs);
        }

    private:
        pointer m_ptr;
    };

    // TODO reverse iterator

    str() = default;

    str(const char*);

    str(char*&&);

    str(const str&);

    str(str&&);

    ~str()
    { delete[] m_str; }

    str& operator=(str);

    char& operator[](std::size_t i)
    { return m_str[i]; }

    const char& operator[](std::size_t i) const
    { return m_str[i]; }

    friend void swap(str&, str&);

    const char* chars() const
    { return (m_str ? m_str : ""); }

    bool empty() const
    { return this->m_len == 0; }

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

    iterator begin()
    { return iterator(&m_str[0]); }

    iterator end()
    { return iterator(&m_str[m_len]); }

    void push_back(char);

    str& append(std::size_t, char);

    str& append(const str&,
            std::size_t count = std::numeric_limits<std::size_t>::max());

    str& insert(std::size_t, std::size_t, int);

    str& insert(std::size_t, const str&);

    str& replace(std::size_t, std::size_t, std::size_t, char);

    str& resize(std::size_t, char c = '\0');

    str& clear();

    str& erase(std::size_t,
            std::size_t count = std::numeric_limits<std::size_t>::max());

    str& remove_newline();

    // TODO iterators

private:
    char* m_str{nullptr};
    std::size_t m_len{0};
    std::size_t m_size{1};

    void try_realloc_str(std::size_t extra_len);
};
