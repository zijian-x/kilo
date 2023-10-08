#pragma once

#include <bits/types/struct_itimerspec.h>
#include <cstddef>
#include <iterator>
#include <limits>
#include <utility>

class str
{
public:
    using value_type = char;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::contiguous_iterator_tag;
    using iterator = pointer;
    using const_iterator = const_pointer;

    str() = default;

    str(const_pointer);

    str(char*&&);

    str(const str&);

    str(str&&);

    ~str();

    str& operator=(str);

    friend void swap(str&, str&);

    reference operator*()
    { return *bptr; }

    const_reference operator*() const
    { return *bptr; }

    pointer operator->()
    { return bptr; }

    const_pointer operator->() const
    { return bptr; }

    reference operator[](std::size_t i)
    { return bptr[i]; }

    const_reference operator[](std::size_t i) const
    { return bptr[i]; }

    const_pointer c_str() const
    { return bptr; }

    bool empty() const
    { return this->m_size == 0; }

    std::size_t size() const
    { return this->m_size; }

    std::size_t capacity() const
    { return this->m_capacity; }

    reference front()
    { return bptr[0]; }

    reference front() const
    { return const_cast<str&>(*this).front(); }

    reference back()
    { return bptr[m_size - 1]; }

    reference back() const
    { return const_cast<str&>(*this).back(); }

    iterator begin()
    { return bptr; }

    iterator end()
    { return iterator(&bptr[m_size]); }

    const_iterator begin() const
    { return const_iterator(&bptr[0]); }

    const_iterator end() const
    { return const_iterator(&bptr[m_size]); }

    const_iterator cbegin() const
    { return const_iterator(&bptr[0]); }

    const_iterator cend() const
    { return const_iterator(&bptr[0]); }

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

    str& erase(const_iterator pos);

    str& erase(const_iterator, const_iterator);

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
