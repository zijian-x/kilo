#pragma once

#include "str.hpp"
#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

class editor_row
{
public:
    editor_row() = default;

    template<typename T>
    editor_row(T&& t)
    {
        m_row = std::forward<T>(t);
        m_render = m_row;
        for (std::size_t i = 0; i < m_render.len(); ++i) {
            if (m_render[i] == '\t')
                m_render.insert(i, 8, ' ');
        }
    }

private:
    str m_row;
    str m_render;
};

class editor_state
{
public:
    editor_state();

    std::size_t& screen_row()
    { return this->m_screen_row; }

    const std::size_t& screen_row() const
    { return this->m_screen_row; }

    std::size_t& screen_col()
    { return this->m_screen_col; }

    const std::size_t& screen_col() const
    { return this->m_screen_col; }

    std::size_t& c_row()
    { return this->m_c_row; }

    const std::size_t& c_row() const
    { return this->m_c_row; }

    std::size_t& c_col()
    { return this->m_c_col; }

    const std::size_t& c_col() const
    { return this->m_c_col; }

    std::size_t& rowoff()
    { return this->m_rowoff; }

    const std::size_t& rowoff() const
    { return this->m_rowoff; }

    std::vector<str>& content()
    { return this->m_content; }

    const std::vector<str>& content() const
    { return const_cast<editor_state&>(*this).content(); }

    std::size_t& coloff()
    { return this->m_coloff; }

    const std::size_t& coloff() const
    { return this->m_coloff; }

    void move_curor(int c);

private:
    std::size_t m_screen_row{}, m_screen_col{};
    std::size_t m_c_row{}, m_c_col{};
    std::size_t m_rowoff{}, m_coloff{};
    std::vector<str> m_content;
};