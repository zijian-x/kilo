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
    editor_row(size_t len) noexcept;
    ~editor_row();
    editor_row(const editor_row&) noexcept;
    editor_row(editor_row&&) noexcept;
    editor_row& operator=(editor_row);
    friend void swap(editor_row&, editor_row&);
    // TODO subscript opr
    inline char* chars() { return this->m_chars; }
    inline const char* chars() const { return this->m_chars; }
    inline size_t& len() { return this->m_len; }
    inline const size_t& len() const { return this->m_len; }

private:
    char* m_chars;
    size_t m_len;
};

class editor_content
{
public:
    editor_content() noexcept : editor_content(0) {}
    editor_content(size_t) noexcept;
    inline editor_row* rows() { return this->m_ed_rows; }
    inline const editor_row* rows() const { return this->m_ed_rows; }
    inline size_t& size() { return this->m_size; }
    inline const size_t& size() const { return this->m_size; }

private:
    editor_row* m_ed_rows;
    size_t m_size;
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
    inline editor_content& content() { return this->m_content; }
    inline const editor_content& content() const { return const_cast<editor_state&>(*this).content(); }
    void move_curor(int c);

private:
    unsigned int m_screen_row, m_screen_col;
    unsigned int m_c_row, m_c_col;
    editor_content m_content;

    std::optional<std::pair<unsigned int, unsigned int>> get_win_size();
    void set_win_size();
};
