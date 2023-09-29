#pragma once

#include "str.hpp"
#include <vector>

class editor_content
{
private:
    editor_content() : m_rows{}, m_size{} {}
    ~editor_content();
    editor_content(const editor_content&);
    editor_content(editor_content&&);
    editor_content& operator=(editor_content);
    friend void swap(editor_content&, editor_content&);

    void push_back(str);

    str* m_rows;
    size_t m_size;
};

class editor_state
{
public:
    editor_state();

    inline std::size_t& screen_row()
    { return this->m_screen_row; }

    inline const std::size_t& screen_row() const
    { return this->m_screen_row; }

    inline std::size_t& screen_col()
    { return this->m_screen_col; }

    inline const std::size_t& screen_col() const
    { return this->m_screen_col; }

    inline std::size_t& c_row()
    { return this->m_c_row; }

    inline const std::size_t& c_row() const
    { return this->m_c_row; }

    inline std::size_t& c_col()
    { return this->m_c_col; }

    inline const std::size_t& c_col() const
    { return this->m_c_col; }

    inline std::size_t& rowoff()
    { return this->m_rowoff; }

    inline const std::size_t& rowoff() const
    { return this->m_rowoff; }

    inline std::vector<str>& content()
    { return this->m_content; }

    inline const std::vector<str>& content() const
    { return const_cast<editor_state&>(*this).content(); }

    inline std::size_t& coloff()
    { return this->m_coloff; }

    inline const std::size_t& coloff() const
    { return this->m_coloff; }

    void move_curor(int c);

private:
    std::size_t m_screen_row{}, m_screen_col{};
    std::size_t m_c_row{}, m_c_col{};
    std::size_t m_rowoff{}, m_coloff{};
    std::vector<str> m_content;
};
