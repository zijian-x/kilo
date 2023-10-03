#pragma once

#include "str.hpp"
#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

static constexpr unsigned short TABSTOP = 8;

class editor_row
{
public:
    editor_row() = default;

    template<typename T>
    editor_row(T&& t)
    {
        m_row = std::forward<T>(t);
        m_render = m_row;
        render_row();
    }

    const str& content() const
    { return this->m_row; }

    const str& render() const
    { return this->m_render; }

    std::size_t c_col_to_r_col(std::size_t);

private:
    str m_row;
    str m_render;

    void render_row();
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

    std::size_t& r_col()
    { return this->m_r_col; }

    const std::size_t& r_col() const
    { return this->m_r_col; }

    std::size_t& c_col()
    { return this->m_c_col; }

    const std::size_t& c_col() const
    { return this->m_c_col; }

    std::size_t& rowoff()
    { return this->m_rowoff; }

    const std::size_t& rowoff() const
    { return this->m_rowoff; }

    std::vector<editor_row>& content()
    { return this->m_content; }

    const std::vector<editor_row>& content() const
    { return this->m_content; }

    std::size_t& coloff()
    { return this->m_coloff; }

    const std::size_t& coloff() const
    { return this->m_coloff; }

    void move_curor(int c);

private:
    std::size_t m_screen_row{}, m_screen_col{};
    std::size_t m_c_row{}, m_c_col{}, m_r_col{};
    std::size_t m_rowoff{}, m_coloff{};
    std::vector<editor_row> m_content;
};
