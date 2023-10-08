#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>
#include <chrono>

#include "str.hpp"

static constexpr unsigned short TABSTOP = 8;
static constexpr unsigned short QUIT_TIMES = 1;
static constexpr const char* DEFAULT_MSG = "HELP: CTRL-S = save | "
                                           "CTRL-Q = Quit";

class status_message
{
public:
    status_message() = default;

    status_message(str msg)
    { set_msg(std::move(msg)); }

    const str& msg() const
    { return this->m_msg; }

    const std::chrono::time_point<std::chrono::system_clock>& timestamp() const
    { return this->m_timestamp; }

    void set_msg(str msg)
    {
        this->m_msg = std::move(msg);
        this->m_timestamp = std::chrono::system_clock::now();
    }

private:
    str m_msg = DEFAULT_MSG;
    std::chrono::time_point<std::chrono::system_clock> m_timestamp = std::chrono::system_clock::now(); // wtf man..., i just want to get the time
};

class editor_state
{
public:
    editor_state();

    str& filename()
    { return this->m_filename; }

    const str& filename() const
    { return this->m_filename; }

    std::size_t& dirty()
    { return this->m_dirty; }

    const std::size_t& dirty() const
    { return this->m_dirty; }

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

    std::size_t& coloff()
    { return this->m_coloff; }

    const std::size_t& coloff() const
    { return this->m_coloff; }

    std::vector<str>& content()
    { return this->m_content; }

    const std::vector<str>& content() const
    { return this->m_content; }

    status_message& status_msg()
    { return this->m_status_msg; }

    const status_message& status_msg() const
    { return this->m_status_msg; }

    void move_curor(int);

    std::size_t c_col_to_r_col(const str& row, std::size_t);

    void insert_char(int);

    void delete_char();

    void insert_newline();

    str rows_to_string() const;

private:

    str m_filename{};
    std::size_t m_dirty{};
    std::size_t m_screen_row{}, m_screen_col{};
    std::size_t m_c_row{}, m_c_col{}, m_r_col{};
    std::size_t m_rowoff{}, m_coloff{};
    std::vector<str> m_content; // TODO replace vector with custom class
    status_message m_status_msg;
};

void quit_editor();
