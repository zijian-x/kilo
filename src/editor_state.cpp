#include "editor_state.h"

editor_row::editor_row() noexcept
    : m_size{0}
    , m_chars{nullptr}
{}

editor_row::~editor_row()
{
    delete[] m_chars;
}

editor_row::editor_row(const editor_row& rhs) noexcept
    : m_size{rhs.m_size}
    , m_chars{new char[m_size]{}}
{
    std::memcpy(m_chars,rhs.m_chars, m_size);
}

editor_row::editor_row(editor_row&& rhs) noexcept
    : m_size{rhs.m_size}
    , m_chars{std::exchange(rhs.m_chars, nullptr)}
{}

editor_row& editor_row::operator=(editor_row rhs)
{
    swap(*this, rhs);
    return *this;
}

void swap(editor_row& lhs, editor_row& rhs)
{
    using std::swap;

    swap(lhs.m_size, rhs.m_size);
    swap(lhs.m_chars, rhs.m_chars);
}

editor_state::editor_state() noexcept
    : m_c_row{0}
    , m_c_col{0}
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
        case editor_key::DEL:
            // TODO
            break;
    }
}

std::optional<std::pair<unsigned int, unsigned int>>
editor_state::get_win_size()
{
    winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_row == 0)
        return {};
    return std::make_pair(ws.ws_row, ws.ws_col);
}

void editor_state::set_win_size()
{
    auto ws = get_win_size();
    if (!ws.has_value())
        die("get_win_size");

    auto [row, col] = ws.value();
    m_screen_row = row;
    m_screen_col = col;
}
