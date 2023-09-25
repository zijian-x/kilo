#pragma once

#include <optional>
#include <sys/ioctl.h>
#include <unistd.h>
#include <utility>
#include <cstring>
#include <utility>

class editor_row
{
public:
    editor_row() noexcept;
    ~editor_row();
    editor_row(const editor_row&) noexcept;
    editor_row(editor_row&&) noexcept;
    editor_row& operator=(editor_row);
    friend void swap(editor_row&, editor_row&);

private:
    size_t m_size;
    char* m_chars;
};

class editor_state
{
public:
    editor_state() noexcept;
    inline unsigned int screen_row() { return this->m_screen_row; }
    inline unsigned int screen_col() { return this->m_screen_col; }
    inline unsigned int c_row() { return this->m_c_row; }
    inline unsigned int c_col() { return this->m_c_col; }
    void move_curor(int c);

private:
    unsigned int m_screen_row, m_screen_col;
    unsigned int m_c_row, m_c_col;
    editor_row e_row;

    std::optional<std::pair<unsigned int, unsigned int>> get_win_size();
    void set_win_size();
};
