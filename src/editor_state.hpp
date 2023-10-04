#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>
#include <chrono>

#include "str.hpp"

static constexpr unsigned short TABSTOP = 8;
static constexpr const char* DEFAULT_MSG = "HELP: CTRL-Q = Quit";

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

class status_message
{
public:
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

    std::vector<editor_row>& content()
    { return this->m_content; }

    const std::vector<editor_row>& content() const
    { return this->m_content; }

    status_message& status_msg()
    { return this->m_status_msg; }

    const status_message& status_msg() const
    { return this->m_status_msg; }

    void move_curor(int c);

private:

    str m_filename = "[No Name]";
    std::size_t m_screen_row{}, m_screen_col{};
    std::size_t m_c_row{}, m_c_col{}, m_r_col{};
    std::size_t m_rowoff{}, m_coloff{};
    std::vector<editor_row> m_content;
    status_message m_status_msg;
};
