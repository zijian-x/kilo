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

        reference operator[](std::size_t i)
        { return m_ptr[i]; }

        const_reference operator[](std::size_t i) const
        { return m_ptr[i]; }

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

        iterator& operator+=(difference_type diff)
        {
            m_ptr += diff;
            return *this;
        }

        iterator& operator-=(difference_type diff)
        {
            m_ptr -= diff;
            return *this;
        }

        friend iterator operator+(iterator lhs, difference_type diff)
        {
            lhs += diff;
            return lhs;
        }

        friend iterator operator+(difference_type diff, iterator rhs)
        {
            rhs += diff;
            return rhs;
        }

        friend iterator operator-(iterator lhs, difference_type diff)
        {
            lhs -= diff;
            return lhs;
        }

        friend difference_type operator-(const iterator& lhs, const iterator& rhs)
        {
            return lhs.m_ptr - rhs.m_ptr;
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

    // TODO reverse & const iterator

    str() = default;

    str(const char*);

    str(char*&&);

    str(const str&);

    str(str&&);

    ~str();

    str& operator=(str);

    char& operator[](std::size_t i)
    { return bptr[i]; }

    const char& operator[](std::size_t i) const
    { return bptr[i]; }

    friend void swap(str&, str&);

    const char* c_str() const
    { return bptr; }

    bool empty() const
    { return this->m_size == 0; }

    std::size_t size() const
    { return this->m_size; }

    std::size_t capacity() const
    { return this->m_capacity; }

    char& front()
    { return bptr[0]; }

    char& front() const
    { return const_cast<str&>(*this).front(); }

    char& back()
    { return bptr[m_size - 1]; }

    char& back() const
    { return const_cast<str&>(*this).back(); }

    iterator begin()
    { return iterator(&bptr[0]); }

    iterator end()
    { return iterator(&bptr[m_size]); }

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

    str& erase(iterator pos);

    str& erase(iterator, iterator);

    str& remove_newline();

private:
    static constexpr unsigned int SBO_SIZE = 15;
    std::size_t m_capacity{SBO_SIZE};
    std::size_t m_size{0};
    union {
        char smb[15]{};
        char* dynb;
    };
    bool sbo{true};
    char* bptr{smb};

    void ensure_capacity(std::size_t extra_len);
};
