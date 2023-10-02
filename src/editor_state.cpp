#include "editor_state.hpp"
#include "die.hpp"
#include "keycode.hpp"

#include <sys/ioctl.h>
#include <utility>

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
            if (m_c_col)
                --m_c_col;
            else if (m_coloff)
                --m_coloff;
            // TODO move left to the end of the prev line
            break;
        case editor_key::DOWN:
            // TODO refactor to a function taking $n$ row
            if (m_c_row < m_screen_row - 1)
                ++m_c_row;
            else if (m_rowoff < m_content.size() - m_screen_row)
                ++m_rowoff;
            break;
        case editor_key::UP:
            if (m_c_row)
                --m_c_row;
            else if (m_rowoff)
                --m_rowoff;
            break;
        case editor_key::RIGHT:
            if (m_c_col < m_screen_col - 1)
                ++m_c_col;
            else if (m_coloff < m_content[m_c_row].render().len() - m_screen_col)
                ++m_coloff;
            // TODO move right to the beginning of the next line
            break;
        case editor_key::PAGE_UP:
            m_c_row = 0; // FIXME move cursor to the top/bottom for now
            break;
        case editor_key::PAGE_DOWN:
            while (m_c_row < m_screen_row - 1) // FIXME
                ++m_c_row;
        case editor_key::HOME:
            m_c_col = 0;
            break;
        case editor_key::END:
            m_c_col = m_screen_col - 1;
            break;
        case editor_key::DEL: // TODO
            break;
    }
}
