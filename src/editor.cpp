#include "str.hpp"
#include "editor.hpp"
#include "editor_keys.hpp"
#include "read_input.hpp"

#include <cstddef>
#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace char_seq;

editor_row::editor_row(const str& s) : m_content{s}
{ upd_row(); }

editor_row::editor_row(str&& s) : m_content{s}
{ upd_row(); }

void editor_row::upd_row()
{
    render_content();
    hl_content();
}

void editor_row::render_content()
{
    m_render = m_content;
    for (size_t i = 0; i < m_render.size(); ++i) {
        if (m_content[i] == '\t')
            m_render.replace(i, 1, TABSTOP, ' ');
    }
}

void editor_row::hl_content()
{
    auto is_sep = [](char c) {
        return isspace(c)
            || c == '\0'
            || str(",.()+-/*=~%<>[];").find(c) != str::npos;
    };
    m_hl.resize(m_render.size(), colors::DEFAULT);
    for (size_t i = 0; i < m_render.size(); ++i) {
        auto color = colors::DEFAULT;
        if (std::isdigit(m_render[i]))
            color = colors::CYAN;
        m_hl[i] = static_cast<char>(color);
    }
}

void editor_row::insert(str::size_type index, str::size_type count, int c)
{
    m_content.insert(index, count, c);
    m_render.insert(index, count, c);
    upd_row();
}

void editor_row::erase(str::size_type index, str::size_type count)
{
    m_content.erase(index, count);
    m_render.erase(index, count);
    upd_row();
}

void editor_row::append(const editor_row& row)
{
    m_content.append(row.content());
    m_render.append(row.content());
    upd_row();
}


editor::editor()
{
    winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_row == 0)
        throw std::runtime_error("ioctl error");
    m_screen_row = ws.ws_row - 2;
    m_screen_col = ws.ws_col;
}

void editor::move_curor(int c)
{
    switch (c) {
        case editor_key::LEFT:
            if (m_c_col) {
                --m_c_col;
            } else if (m_c_row > 0) {
                --m_c_row;
                m_c_col = m_rows[m_c_row].size();
            }
            break;
        case editor_key::RIGHT:
            if (m_c_row < m_rows.size()) {
                if (m_c_col < m_rows[m_c_row].size())
                    ++m_c_col;
                else
                    ++m_c_row, m_c_col = 0;
            }
            break;
        case editor_key::UP:
            if (m_c_row)
                --m_c_row;
            break;
        case editor_key::DOWN:
            if (m_c_row < m_rows.size())
                ++m_c_row;
            break;
    }

    m_c_col = std::min(m_c_col,
            m_c_row < m_rows.size() ? m_rows[m_c_row].size() : 0);
}

void editor::set_r_col()
{
    m_r_col = 0;
    const auto& row = m_rows[m_c_row];
    for (size_t i = 0; i < m_c_col; ++i) {
        if (row.content()[i] == '\t')
            m_r_col += (TABSTOP - 1) - (m_r_col % TABSTOP);
        ++m_r_col;
    }
}

void editor::insert_char(int c)
{
    if (m_c_row == m_rows.size())
        m_rows.push_back(str());

    m_rows[m_c_row].insert(m_c_col++, 1, c);
    ++m_dirty;
}

void editor::delete_char()
{
    if (m_c_row == m_rows.size())
        return;

    auto& current_row = m_rows[m_c_row];
    if (m_c_col) {
        current_row.erase(m_c_col - 1, 1);
        --m_c_col;
        ++m_dirty;
    } else if (m_c_row) {
        auto& prev_row = m_rows[m_c_row - 1];
        m_c_col = prev_row.content().size();
        prev_row.append(current_row);
        m_rows.erase(begin(m_rows) + static_cast<long>(m_c_row));
        --m_c_row;
        ++m_dirty;
    }
}

void editor::insert_newline()
{
    auto c_row_iter = begin(m_rows) + static_cast<ptrdiff_t>(m_c_row);
    if (!m_c_col) {
        m_rows.emplace(c_row_iter, str());
    } else {
        auto new_row = str(c_row_iter->content().begin() + m_c_col, c_row_iter->content().end());
        c_row_iter->content().erase(c_row_iter->content().begin() + m_c_col, c_row_iter->content().end());
        c_row_iter->upd_row();
        m_rows.emplace(c_row_iter + 1, std::move(new_row));
        m_c_col = 0;
    }
    ++m_c_row;
    ++m_dirty;
}

void editor::incr_find(const str& query, int key)
{
    enum class direction { FORWARD, BACKWARD };
    static size_t last_match_row = 0;
    static size_t last_match_col = str::npos;
    static direction dir = direction::FORWARD;

    static size_t cached_hl_row = str::npos;
    if (cached_hl_row != str::npos) {
        m_rows[cached_hl_row].upd_row();
        cached_hl_row = str::npos;
    }

    if (m_rows.empty())
        return;
    if (key == editor_key::ESCAPE || key == '\r') {
        last_match_row = 0;
        last_match_col = str::npos;
        dir = direction::FORWARD;
        return;
    } else if (key == editor_key::UP) {
        dir = direction::BACKWARD;
    } else if (key == editor_key::DOWN) {
        dir = direction::FORWARD;
    } else {
        last_match_row = 0;
        last_match_col = str::npos;
        dir = direction::FORWARD;
    }

    auto cur_row = last_match_row;
    auto cur_col = last_match_col;
    do {
        switch (dir) {
            case direction::FORWARD:
                cur_col += 1;
                break;
            case direction::BACKWARD:
                cur_col -= 1;
                break;
        }
        if (cur_col == m_rows[cur_row].size()) {
            cur_row = (cur_row + 1) % m_rows.size();
            cur_col = 0;
        }
        if (cur_col == str::npos) {
            cur_row = std::min(cur_row - 1, m_rows.size() - 1);
            cur_col = 0;
        }

        auto& row = m_rows[cur_row];
        if (dir == direction::FORWARD) {
            if (auto pos = row.render().find(query, cur_col); pos != str::npos) {
                m_c_row = last_match_row = cur_row;
                m_c_col = last_match_col = pos;
                row.hl().replace(pos, query.size(), query.size(), colors::RED);
                cached_hl_row = cur_row;
                return;
            } else {
                cur_row = (cur_row + 1) % m_rows.size();
                cur_col = str::npos;
            }
        }
        if (dir == direction::BACKWARD) {
            if (auto pos = row.render().rfind(query, cur_col); pos != str::npos) {
                m_c_row = last_match_row = cur_row;
                m_c_col = last_match_col = pos;
                row.hl().replace(pos, query.size(), query.size(), colors::RED);
                cached_hl_row = cur_row;
                return;
            } else {
                cur_row = std::min(cur_row - 1, m_rows.size() - 1);
                cur_col = m_rows[cur_row].size();
            }
        }
    } while (cur_row != last_match_row);
}

void editor::find()
{
    auto cache_row = m_c_row;
    auto cache_col = m_c_col;
    auto cache_rowoff = m_rowoff;
    auto cache_coloff = m_coloff;
    auto callback = std::function<void(editor&, const str&, int)>
        (&editor::incr_find);

    auto query = prompt_input(*this, "Search: ", callback);
    if (!query.empty())
        return;

    m_c_row = cache_row;
    m_c_col = cache_col;
    m_rowoff = cache_rowoff;
    m_coloff = cache_coloff;
    m_status_msg.set_content("Search aborted");
}

str editor::rows_to_string() const
{
    auto buf = str();
    for (const auto& line : m_rows) {
        buf.append(line.content());
        buf.push_back('\n');
    }

    return buf;
}

void quit_editor()
{
    write(STDOUT_FILENO, esc_seq::CLEAR_SCREEN, 4);
    write(STDOUT_FILENO, esc_seq::CLEAR_CURSOR_POS, 3);
    std::exit(0);
}
