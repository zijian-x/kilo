#pragma once

#include <array>
#include <string_view>
#include <bitset>
#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>
#include <chrono>

#include "str.hpp"

#define HL_NUMBER (1<<0)
#define HL_STRING (1<<1)

static constexpr unsigned short TABSTOP = 8;
static constexpr unsigned short QUIT_TIMES = 1;
static constexpr std::string_view DEFAULT_MSG = "HELP: CTRL-S = save"
                                           " | CTRL-Q = Quit"
                                           " | CTRL-F = Find";

struct editor_syntax
{
    str filetype;
    std::vector<str> filematches;
    str single_line_comment_syntax;
    unsigned int flags;
};

static const inline std::array HLDB{
    editor_syntax{ "c", {".c", ".cpp", ".h"}, "//", HL_NUMBER | HL_STRING },
};

class editor_row
{
public:
    editor_row() = default;

    editor_row(const str& s, std::optional<editor_syntax> hl_syntax = {});

    editor_row(str&& s, std::optional<editor_syntax> hl_syntax = {});

    const str& content() const
    { return this->m_content; }

    str& content()
    { return this->m_content; }

    const str& render() const
    { return this->m_render; }

    str& render()
    { return this->m_render; }

    const str& hl() const
    { return this->m_hl; }

    str& hl()
    { return this->m_hl; }

    std::optional<editor_syntax>& hl_syntax()
    { return this->m_hl_syntax; }

    const std::optional<editor_syntax>& hl_syntax() const
    { return this->m_hl_syntax; }

    str::size_type size() const
    { return m_content.size(); }

    void insert(str::size_type, str::size_type, int);

    void erase(str::size_type, str::size_type);

    void append(const editor_row&);

    void upd_row();

private:
    str m_content;
    str m_render;
    str m_hl;
    std::optional<editor_syntax> m_hl_syntax;

    void render_content();
    void hl_content();
};

class status_message
{
public:
    status_message() = default;

    status_message(str msg)
    { set_content(std::move(msg)); }

    const str& content() const
    { return this->m_content; }

    const std::chrono::time_point<std::chrono::system_clock>& timestamp() const
    { return this->m_timestamp; }

    void set_content(str content)
    {
        this->m_content = std::move(content);
        this->m_timestamp = std::chrono::system_clock::now();
    }

    void clear()
    {
        this->m_content.clear();
        this->m_timestamp = std::chrono::system_clock::now();
    }

private:
    str m_content = DEFAULT_MSG.data();
    std::chrono::time_point<std::chrono::system_clock> m_timestamp = std::chrono::system_clock::now(); // wtf man..., i just want to get the time
};

class editor
{
public:
    editor();

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

    std::vector<editor_row>& rows()
    { return this->m_rows; }

    const std::vector<editor_row>& rows() const
    { return this->m_rows; }

    status_message& status_msg()
    { return this->m_status_msg; }

    const status_message& status_msg() const
    { return this->m_status_msg; }

    std::optional<editor_syntax>& hl_syntax()
    { return this->m_hl_syntax; }

    const std::optional<editor_syntax>& hl_syntax() const
    { return this->m_hl_syntax; }

    void set_ft();

    void move_curor(int);

    void set_r_col();

    void insert_char(int);

    void delete_char();

    void insert_newline();

    void find();

    str rows_to_string() const;

private:

    str m_filename{};
    std::size_t m_dirty{};
    std::size_t m_screen_row{}, m_screen_col{};
    std::size_t m_c_row{}, m_c_col{}, m_r_col{};
    std::size_t m_rowoff{}, m_coloff{};
    std::vector<editor_row> m_rows;
    status_message m_status_msg;
    std::optional<editor_syntax> m_hl_syntax;

    void incr_find(const str&, int);
};

void quit_editor();
