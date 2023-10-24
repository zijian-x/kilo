#include "str.hpp"
#include "editor.hpp"
#include "editor_keys.hpp"
#include "read_input.hpp"

#include <cstddef>
#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace char_seq;

editor_row::editor_row(const str& s, std::optional<editor_syntax> hl_syntax)
    : m_content{s}
    , m_hl_syntax{hl_syntax}
{ upd_row(); }

editor_row::editor_row(str&& s, std::optional<editor_syntax> hl_syntax)
    : m_content{s}
    , m_hl_syntax{hl_syntax}
{ upd_row(); }

void editor_row::upd_row()
{
    render_content();
    hl_content();
}

void editor_row::render_content()
{
    using std::begin, std::end;
    auto tab_cnt = std::count(begin(m_content), end(m_content), '\t');

    m_render.clear();
    m_render.reserve(m_content.size() + static_cast<size_t>(tab_cnt * (TABSTOP - 1)) + 1);

    size_t idx = 0;
    for (auto c : m_content) {
        if (c == '\t') {
            auto cnt = TABSTOP - idx % TABSTOP;
            m_render.append(cnt, ' ');
            idx += cnt - 1;
        } else {
            m_render.push_back(c);
        }
        ++idx;
    }
}

void editor_row::hl_content()
{
    m_hl.resize(m_render.size(), colors::DEFAULT);
    if (!m_hl_syntax.has_value())
        return;

    bool prev_is_sep = true;
    char in_string = 0;
    for (size_t i = 0; i < m_render.size(); ++i) {
        static auto is_sep = [](char c) {
            return isspace(c)
                || c == '\0'
                || str(",.()+-/*=~%<>[];'\"").find(c) != str::npos;
        };
        auto prev_color = (i) ? m_hl[i - 1] : colors::DEFAULT;
        auto cur_color = colors::DEFAULT;
        auto hl_nums = [&](char c) {
            return (m_hl_syntax.has_value() &&
                    (m_hl_syntax.value().flags & HL_NUMBER) &&
                    std::isdigit(c)
                    && (prev_is_sep || prev_color == colors::CYAN))
                || (c == '.' && prev_color == colors::CYAN);

        };
        auto hl_string = [&](char c) {
            if (!m_hl_syntax.has_value() || !(m_hl_syntax.value().flags & HL_STRING))
                return false;
            if (in_string) {
                if (c == in_string && i && m_render[i - 1] != '\\')
                    in_string = 0;
                return true;
            } else if (c == '"' || c == '\'') {
                in_string = c;
                return true;
            }
            return false;
        };

        if (hl_string(m_render[i])) {
            cur_color = colors::YELLOW;
        } else if (hl_nums(m_render[i]))
            cur_color = colors::CYAN;
        m_hl[i] = static_cast<char>(cur_color);

        prev_is_sep = is_sep(m_render[i]);
        prev_color = cur_color;
    }
}

void editor_row::insert(str::size_type index, str::size_type count, int c)
{
    m_content.insert(index, count, c);
    upd_row();
}

void editor_row::erase(str::size_type index, str::size_type count)
{
    m_content.erase(index, count);
    upd_row();
}

void editor_row::append(const editor_row& row)
{
    m_content.append(row.content());
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

void editor::set_ft()
{
    if (m_filename.empty())
        return;

    auto file_ext = m_filename.rfind('.');
    for (const auto& hl_syntax : HLDB) {
        for (const auto& ft : hl_syntax.filematches) {
            size_t i = 0;
            while (i < ft.size() && file_ext + i < m_filename.size()
                    && m_filename[file_ext + i] == ft[i])
                ++i;
            if (i == ft.size()) {
                m_hl_syntax = hl_syntax;
                for (auto& row : m_rows) {
                    row.hl_syntax() = m_hl_syntax;
                    row.upd_row();
                }
                return;
            }
        }
    }
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
