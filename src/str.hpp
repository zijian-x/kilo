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
    using size_type = std::size_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::contiguous_iterator_tag;
    using iterator = pointer;
    using const_iterator = const_pointer;

    static constexpr size_type npos = static_cast<size_type>(-1);

    str() = default;

    str(const_pointer);

    str(const str&);

    str(str&&);

    str(const_iterator, const_iterator);

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

    reference operator[](size_type i)
    { return bptr[i]; }

    const_reference operator[](size_type i) const
    { return bptr[i]; }

    const_pointer c_str() const
    { return bptr; }

    bool empty() const
    { return !this->m_size; }

    size_type size() const
    { return this->m_size; }

    size_type capacity() const
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

    void push_back(value_type);

    void pop_back();

    str& append(size_type, value_type);

    str& append(const str&, size_type count = npos);

    template<typename input_iter>
    str& append(input_iter first, input_iter last)
    {
        while (first != last)
            this->append(1, *first++);

        return *this;
    }

    str& insert(size_type, size_type, int);

    str& insert(size_type, const str&);

    str& replace(size_type, size_type, size_type, value_type);

    str& resize(size_type, value_type c = '\0');

    str& clear();

    str& erase(size_type, size_type count = npos);

    str& erase(const_iterator pos);

    str& erase(const_iterator, const_iterator);

    size_type find(const str&) const;

    str& remove_newline();

private:
    static constexpr unsigned int SBO_SIZE = 15;
    size_type m_capacity{SBO_SIZE};
    size_type m_size{0};
    union {
        value_type smb[15]{};
        value_type* dynb;
    };
    bool sbo{true};
    value_type* bptr{smb};

    void ensure_capacity(size_type);
};
