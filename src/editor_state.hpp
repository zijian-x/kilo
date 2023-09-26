#pragma once

#include "str.hpp"

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
    inline unsigned int& screen_row() { return this->m_screen_row; }
    inline const unsigned int& screen_row() const { return this->m_screen_row; }
    inline unsigned int& screen_col() { return this->m_screen_col; }
    inline const unsigned int& screen_col() const { return this->m_screen_col; }
    inline unsigned int& c_row() { return this->m_c_row; }
    inline const unsigned int& c_row() const { return this->m_c_row; }
    inline unsigned int& c_col() { return this->m_c_col; }
    inline const unsigned int& c_col() const { return this->m_c_col; }
    void move_curor(int c);

private:
    unsigned int m_screen_row, m_screen_col;
    unsigned int m_c_row, m_c_col;

    void set_win_size();
};
