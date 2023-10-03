#include <gtest/gtest.h>
#include <fmt/core.h>

#include "../src/editor_state.hpp"

class test_editor_row : public ::testing::Test
{
protected:
    editor_row row;
};

TEST_F(test_editor_row, ctor1)
{
    str s = str("hello, world");
    row = std::move(s);

    ASSERT_STREQ(row.content().chars(), row.render().chars());
}

TEST_F(test_editor_row, ctor2)
{
    str s = str("hello\tworld\t\t!");
    row = s;

    for (size_t i = 0; i < s.len(); ++i)
        if (s[i] == '\t')
            s.replace(i, 1, 8, ' ');

    ASSERT_STREQ(s.chars(), row.render().chars());
}
