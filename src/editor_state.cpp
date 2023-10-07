#include "editor_state.hpp"
#include "keycode.hpp"

#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>

editor_state::editor_state()
{
    winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_row == 0)
        throw std::runtime_error("ioctl error");
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
                m_c_col = m_content[m_c_row].size();
            }
            break;
        case editor_key::RIGHT:
            if (m_c_row < m_content.size()) {
                if (m_c_col < m_content[m_c_row].size())
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
            m_c_row < m_content.size() ? m_content[m_c_row].size() : 0);
}

std::size_t editor_state::c_col_to_r_col(const str& row, size_t c_col)
{
    size_t r_col = 0;
    for (size_t i = 0; i < c_col; ++i) {
        if (row[i] == '\t')
            r_col += (TABSTOP - 1) - (r_col % TABSTOP);
        ++r_col;
    }
    return r_col;
}

void editor_state::insert_char(int c)
{
    if (m_c_row == m_content.size())
        m_content.push_back(str());
    m_content[m_c_row].insert(m_c_col++, 1, c);

    ++m_dirty;
}

void editor_state::delete_char()
{
    if (m_c_row == m_content.size())
        return;

    auto& current_row = m_content[m_c_row];
    if (m_c_col) {
        current_row.erase(m_c_col - 1, 1);
        --m_c_col;
        ++m_dirty;
    } else if (m_c_row) {
        auto& prev_row = m_content[m_c_row - 1];
        m_c_col = prev_row.size();
        prev_row.append(current_row);
        m_content.erase(begin(m_content) + static_cast<long>(m_c_row));
        --m_c_row;
        ++m_dirty;
    }
}

str editor_state::rows_to_string() const
{
    auto buf = str();
    for (const auto& line : m_content) {
        buf.append(line);
        buf.push_back('\n');
    }

    return buf;
}

void quit_editor()
{
    write(STDOUT_FILENO, esc_char::CLEAR_SCREEN, 4);
    write(STDOUT_FILENO, esc_char::CLEAR_CURSOR_POS, 3);
    std::exit(0);
}
