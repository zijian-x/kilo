#pragma once

#include <optional>
#include <sys/ioctl.h>
#include <unistd.h>
#include <utility>
#include <cstring>
#include <utility>
#include <vector>

class editor_row
{
public:
    editor_row(size_t len) noexcept;
    editor_row(const char* str) noexcept;
    ~editor_row();
    editor_row(const editor_row&) noexcept;
    editor_row(editor_row&&) noexcept;
    editor_row& operator=(editor_row);
    friend void swap(editor_row&, editor_row&);
    // TODO subscript optr

    inline const char* chars() const { return this->m_chars; }
    inline size_t& len() { return this->m_len; }
    inline const size_t& len() const { return this->m_len; }

private:
    char* m_chars;
    size_t m_len;
};

class editor_content
{
private:
    editor_content() noexcept : rows{}, size{} {}
    ~editor_content();
    editor_content(const editor_content&) noexcept;
    editor_content(editor_content&&) noexcept;
    editor_content& operator=(editor_content) noexcept;
    friend void swap(editor_content&, editor_content&);

    void push_back(editor_row);

    editor_row** rows;
    size_t size;
};

class editor_state
{
public:
    editor_state() noexcept;
    inline unsigned int& screen_row() { return this->m_screen_row; }
    inline const unsigned int& screen_row() const { return this->m_screen_row; }
    inline unsigned int& screen_col() { return this->m_screen_col; }
    inline const unsigned int& screen_col() const { return this->m_screen_col; }
    inline unsigned int& c_row() { return this->m_c_row; }
    inline const unsigned int& c_row() const { return this->m_c_row; }
    inline unsigned int& c_col() { return this->m_c_col; }
    inline const unsigned int& c_col() const { return this->m_c_col; }
    inline std::vector<editor_row>& ed_rows() { return this->m_ed_rows; }
    inline const std::vector<editor_row>& ed_rows() const
    { return const_cast<editor_state&>(*this).ed_rows(); }
    void move_curor(int c);

private:
    unsigned int m_screen_row, m_screen_col;
    unsigned int m_c_row, m_c_col;
    std::vector<editor_row> m_ed_rows;

    void set_win_size();
};
