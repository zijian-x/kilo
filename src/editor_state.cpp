#include "editor_state.hpp"
#include "die.hpp"
#include "keycode.hpp"

#include <sys/ioctl.h>
#include <utility>

void editor_row::render_row()
{
    for (size_t i = 0; i < m_render.len(); ++i) {
        if (m_render[i] == '\t')
            m_render.replace(i, 1, TABSTOP, ' ');
    }
}

std::size_t editor_row::c_col_to_r_col(size_t c_col)
{
    size_t r_col = 0;
    for (size_t i = 0; i < c_col; ++i) {
        if (m_row[i] == '\t')
            r_col += (TABSTOP - 1) - (r_col % TABSTOP);
        ++r_col;
    }
    return r_col;
}

void editor_row::insert_char(size_t index, int c)
{
    m_row.insert(index, 1, c);
    m_render.insert(index, 1, c);

    render_row();
}

editor_state::editor_state()
{
    winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_row == 0)
        die("get_win_size");
    m_screen_row = ws.ws_row - 2;
    m_screen_col = ws.ws_col;
}

void editor_state::move_curor(int c)
{
    switch (c) {
        case editor_key::LEFT:
            if (m_c_col) {
                --m_c_col;
            } else if (m_c_row > 0) {
                --m_c_row;
                m_c_col = m_content[m_c_row].content().len();
            }
            break;
        case editor_key::RIGHT:
            if (m_c_row < m_content.size()) {
                if (m_c_col < m_content[m_c_row].content().len())
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
            if (m_c_row < m_content.size())
                ++m_c_row;
            break;
    }

    m_c_col = std::min(m_c_col,
            m_c_row < m_content.size() ? m_content[m_c_row].content().len() : 0);
}

void editor_state::insert_char(int c)
{
    if (m_c_row == m_content.size())
        m_content.push_back(str());
    m_content[m_c_row].insert_char(m_c_col++, c);
}

str editor_state::rows_to_string() const
{
    auto buf = str();
    for (const auto& line : m_content) {
        buf.append(line.content());
        buf.push_back('\n');
    }

    return buf;
}
