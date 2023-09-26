#include "editor_state.hpp"
#include "utils.hpp"
#include "keycode.hpp"

editor_row::editor_row(size_t len) noexcept
    : m_chars{new char[len + 1]{}}
    , m_len{len}
{}

editor_row::editor_row(const char* str) noexcept
    : m_len{std::strlen(str)}
{
    m_chars = new char[m_len + 1]{};
    std::memcpy(m_chars, str, m_len);
}

editor_row::~editor_row()
{
    delete[] m_chars;
}

editor_row::editor_row(const editor_row& rhs) noexcept
    : m_chars{new char[rhs.m_len]{}}
    , m_len{rhs.m_len}
{
    std::memcpy(m_chars,rhs.m_chars, m_len);
}

editor_row::editor_row(editor_row&& rhs) noexcept
    : m_chars{std::exchange(rhs.m_chars, nullptr)}
    , m_len{rhs.m_len}
{}

editor_row& editor_row::operator=(editor_row rhs)
{
    swap(*this, rhs);
    return *this;
}

void swap(editor_row& lhs, editor_row& rhs)
{
    using std::swap;

    swap(lhs.m_len, rhs.m_len);
    swap(lhs.m_chars, rhs.m_chars);
}

// TODO editor_content

editor_state::editor_state() noexcept
    : m_c_row{}
    , m_c_col{}
{
    set_win_size();
}

void editor_state::move_curor(int c)
{
    switch (c) {
        case editor_key::LEFT:
            if (m_c_col)
                --m_c_col;
            break;
        case editor_key::DOWN:
            if (m_c_row < m_screen_row - 1)
                ++m_c_row;
            break;
        case editor_key::UP:
            if (m_c_row)
                --m_c_row;
            break;
        case editor_key::RIGHT:
            if (m_c_col < m_screen_col - 1)
                ++m_c_col;
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

void editor_state::set_win_size()
{
    winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_row == 0)
        die("get_win_size");
    m_screen_row = ws.ws_row;;
    m_screen_col = ws.ws_col;;
}
