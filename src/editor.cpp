#include "editor.hpp"
#include "editor_keys.hpp"
#include "read_input.hpp"

#include <cstddef>
#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace char_seq;

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
        if (row[i] == '\t')
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
        m_c_col = prev_row.size();
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
        auto new_row = str(c_row_iter->begin() + m_c_col, c_row_iter->end());
        c_row_iter->erase(c_row_iter->begin() + m_c_col, c_row_iter->end());
        m_rows.emplace(c_row_iter + 1, std::move(new_row));
        m_c_col = 0;
    }
    ++m_c_row;
    ++m_dirty;
}

void editor::incr_find(const str& query, int key)
{
    enum direction { FORWARD, BACKWARD };

    static size_t last_match = str::npos;
    static direction dir = direction::FORWARD;

    if (m_rows.empty())
        return;
    if (key == editor_key::ESCAPE || key == '\r') {
        last_match = str::npos;
        dir = direction::FORWARD;
        return;
    } else if (key == editor_key::UP) {
        dir = direction::BACKWARD;
    } else if (key == editor_key::DOWN) {
        dir = direction::FORWARD;
    } else {
        last_match = str::npos;
        dir = direction::FORWARD;
    }

    auto size = m_rows.size();
    auto set_next_row = [&size](size_t cur_row, direction dir) {
        if (dir == direction::FORWARD)
            cur_row = (cur_row + 1) % size;
        else
            cur_row = std::min((cur_row - 1), size);
        return cur_row;
    };
    auto cur_row = set_next_row(m_c_row, dir);
    do {
        if (auto col_pos = m_rows[cur_row].find(query); col_pos != str::npos) {
            last_match = cur_row;
            m_c_row = last_match = cur_row;
            m_c_col = col_pos;
            return;
        }
        cur_row = set_next_row(cur_row, dir);
    } while (cur_row != m_c_row);
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
        buf.append(line);
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