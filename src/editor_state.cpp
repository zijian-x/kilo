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

editor_state::editor_state()
{
    winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_row == 0)
        die("get_win_size");
    m_screen_row = ws.ws_row;;
    m_screen_col = ws.ws_col;;
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
            if (m_c_col < m_content[m_c_row].content().len())
                ++m_c_col;
            else if (m_c_row < m_content.size())
                ++m_c_row, m_c_col = 0;
            break;
        case editor_key::DOWN:
            // TODO refactor to a function taking $n$ row
            if (m_c_row < m_screen_row - 1)
                ++m_c_row;
            break;
        case editor_key::UP:
            if (m_c_row)
                --m_c_row;
            break;
        case editor_key::PAGE_UP:
            m_c_row = 0;
            break;
        case editor_key::PAGE_DOWN:
            while (m_c_row < m_screen_row - 1)
                ++m_c_row;
        case editor_key::HOME:
            m_c_col = 0;
            break;
        case editor_key::END:
            m_c_col = m_content[m_c_row].content().len(); // FIXME move up/down exceeding the next lines len
            break;
        case editor_key::DEL: // TODO
            break;
    }
}
