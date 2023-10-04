#include <algorithm>
#include <fmt/core.h>
#include <gtest/gtest.h>
#include <cstring>
#include <random>
#include <string>

#include "../src/str.hpp"

class test_str : public ::testing::Test
{
protected:
    str s;
    std::string cmp;
    const char* line = "Lorem ipsum dolor sit amet, consectetur:"
                       "adipiscing elit. Quantum Aristoxeni"
                       "ingenium consumptum videmus in musicis?"
                       "Quae similitudo in genere etiam humano"
                       "apparet. Ego vero volo in virtute vim"
                       "esse quam maximam; Egone quaeris,"
                       "inquit, quid sentiam? Primum quid"
                       "tu dicis breve? Facile"
                       "pateremur, qui etiam nunc"
                       "agendi aliquid discendique"
                       "causa prope contra naturam"
                       "vígillas suscipere soleamus.";
    std::mt19937 mt{};
};

TEST_F(test_str, default_ctor)
{
    ASSERT_EQ(s.chars(), nullptr);
}

TEST_F(test_str, char_ptr_ctor)
{
    auto s = str(line);
    ASSERT_STREQ(s.chars(), line);
    ASSERT_NE(s.chars(), line);
    ASSERT_EQ(s.len(), std::strlen(line));
}

TEST_F(test_str, moved_char_ptr_ctor)
{
    auto* ptr = new char[std::strlen(line) + 1]{};
    std::strcpy(ptr, line);
    auto addr = reinterpret_cast<intptr_t>(ptr);

    auto s = str(std::move(ptr));

    ASSERT_EQ(reinterpret_cast<intptr_t>(s.chars()), addr);
    ASSERT_STREQ(s.chars(), line);
}

TEST_F(test_str, copy_ctor)
{
    auto s1 = str();
    auto s2 = str(line);
    s1 = s2;

    ASSERT_STREQ(s1.chars(), s2.chars());
    ASSERT_EQ(s1.len(), s2.len());
    ASSERT_NE(s1.chars(), s2.chars());
}

TEST_F(test_str, move_ctor)
{
    auto s1 = str();
    auto s2 = str(line);
    auto addr = reinterpret_cast<intptr_t>(s2.chars());

    s1 = std::move(s2);

    ASSERT_EQ(addr, reinterpret_cast<intptr_t>(s1.chars()));
    ASSERT_STREQ(s1.chars(), line);
    ASSERT_EQ(s1.len(), std::strlen(line));
}

TEST_F(test_str, opertor_equal)
{
    auto tmp = str(line);
    s = tmp;

    ASSERT_STREQ(s.chars(), tmp.chars());
    ASSERT_NE(s.chars(), tmp.chars());
    ASSERT_EQ(s.len(), tmp.len());
}

TEST_F(test_str, operator_equal_move)
{
    auto tmp = str(line);
    s = std::move(tmp);
}

TEST_F(test_str, Front)
{
    s = str(line);
    ASSERT_EQ(s.front(), line[0]);
}

TEST_F(test_str, back_and_push_back)
{
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        s.push_back(line[i]);
        ASSERT_EQ(s.back(), line[i]);
    }
}

TEST_F(test_str, append)
{
    for (size_t i = 0; i < 5; ++i) {
        s.append(line);
        cmp.append(line);
    }

    ASSERT_STREQ(s.chars(), cmp.c_str());
    ASSERT_EQ(s.len(), cmp.size());

    s.clear();
    cmp.clear();
    ASSERT_STREQ(s.chars(), cmp.c_str());
    ASSERT_EQ(s.len(), cmp.size());
}

TEST_F(test_str, insert1)
{
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        s.insert(i, 1, line[i]);
        cmp.insert(i, 1, line[i]);
    }

    ASSERT_STREQ(s.chars(), cmp.c_str());
    ASSERT_EQ(s.len(), cmp.size());

    s.clear();
    cmp.clear();
    ASSERT_STREQ(s.chars(), cmp.c_str());
    ASSERT_EQ(s.len(), cmp.size());
}

TEST_F(test_str, insert2)
{
    const auto line = std::string{"Lorem ipsum dolor sit amet, consectetur:"
                                  "adipiscing elit. Quantum Aristoxeni"
                                  "ingenium consumptum videmus in musicis?"
                                  "Quae similitudo in genere etiam humano"
                                  "apparet. Ego vero volo in virtute vim"
                                  "esse quam maximam; Egone quaeris,"
                                  "inquit, quid sentiam? Primum quid"
                                  "tu dicis breve? Facile"
                                  "pateremur, qui etiam nunc"
                                  "agendi aliquid discendique"
                                  "causa prope contra naturam"
                                  "vígillas suscipere soleamus."};

    auto line_idx_range = std::uniform_int_distribution<size_t>(0, line.size() - 1);
    auto count_range = std::uniform_int_distribution<size_t>(0, 20);

    for (size_t i = 0; i < line.size(); ++i) {
        auto insert_idx_range = std::uniform_int_distribution<size_t>(0, i);
        auto insert_idx = insert_idx_range(mt);
        auto line_idx = line_idx_range(mt);
        auto cnt = count_range(mt);

        s.insert(insert_idx, cnt, line[line_idx]);
        cmp.insert(insert_idx, cnt, line[line_idx]);
        ASSERT_STREQ(s.chars(), cmp.c_str());
    }

    s.clear();
    cmp.clear();
    ASSERT_STREQ(s.chars(), cmp.c_str());
    ASSERT_EQ(s.len(), cmp.size());
}

TEST_F(test_str, insert_str)
{
    const auto line = str{ "Lorem ipsum dolor sit amet, consectetur:" };

    for (size_t i = 0; i < 50; ++i) {
        auto insert_idx_range = std::uniform_int_distribution<size_t>(0, i);
        auto insert_idx = insert_idx_range(mt);

        s.insert(insert_idx, line);
        cmp.insert(insert_idx, line.chars());

        ASSERT_STREQ(s.chars(), cmp.c_str());
    }

    s.clear();
    cmp.clear();
    ASSERT_STREQ(s.chars(), cmp.c_str());
    ASSERT_EQ(s.len(), cmp.size());
}

TEST_F(test_str, resize1)
{
    s = line;
    cmp = line;
    auto rand = std::uniform_int_distribution<size_t>(0, 100);
    for (size_t i = 0; i < 50; ++i) {
        auto size = rand(mt);
        s.resize(size);
        cmp.resize(size);
        ASSERT_EQ(s.len(), cmp.size());
        ASSERT_STREQ(s.chars(), cmp.c_str());
    }
}

TEST_F(test_str, resize2)
{
    s = line;
    cmp = line;
    auto rand = std::uniform_int_distribution<size_t>(0, 100);
    auto rand_alpha = std::uniform_int_distribution<char>('a', 'z');
    for (size_t i = 0; i < 50; ++i) {
        auto size = rand(mt);
        auto c = rand_alpha(mt);
        s.resize(size, c);
        cmp.resize(size, c);
        ASSERT_EQ(s.len(), cmp.size());
        ASSERT_STREQ(s.chars(), cmp.c_str());
    }
}

TEST_F(test_str, remove_newline)
{
    const auto* buf = "hello, world";
    s = buf;
    s.push_back('\n');
    s.remove_newline();

    ASSERT_STREQ(s.chars(), buf);
    ASSERT_EQ(s.len(), std::strlen(buf));
}

TEST_F(test_str, replace1)
{
    const auto* line = "Lorem ipsum dolor sit amet, consectetur:"
                       "adipiscing elit. Quantum Aristoxeni"
                       "ingenium consumptum videmus in musicis?"
                       "Quae similitudo in genere etiam humano"
                       "apparet. Ego vero volo in virtute vim"
                       "esse quam maximam; Egone quaeris,"
                       "inquit, quid sentiam? Primum quid"
                       "tu dicis breve? Facile"
                       "pateremur, qui etiam nunc"
                       "agendi aliquid discendique"
                       "causa prope contra naturam"
                       "vígillas suscipere soleamus.";

    s = str(line);
    cmp = std::string(line);
    ASSERT_EQ(s.len(), cmp.size());

    auto dis = std::uniform_int_distribution<size_t>{1, 10};
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        auto cnt = dis(mt);
        s.replace(i, cnt, cnt, 'c');
        cmp.replace(i, cnt, cnt, 'c');
        ASSERT_STREQ(s.chars(), cmp.c_str());
        ASSERT_EQ(s.len(), cmp.size());
    }
}

TEST_F(test_str, replace2)
{
    s = line;
    cmp = std::string(s.chars());

    s.replace(2, 2, 4, 'b');
    cmp.replace(2, 2, 4, 'b');

    ASSERT_STREQ(s.chars(), cmp.c_str());
    ASSERT_EQ(s.len(), cmp.size());
}

TEST_F(test_str, replace3)
{
    s = str(line);
    cmp = std::string(line);
    auto dis = std::uniform_int_distribution<size_t>{1, 10};
    auto extra_count_dis = dis;
    auto rand_char = std::uniform_int_distribution<char>{'a', 'z'};
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        auto cnt = dis(mt);
        auto extra = extra_count_dis(mt);
        char c = rand_char(mt);

        s.replace(i, cnt, cnt + extra, c);
        cmp.replace(i, cnt, cnt + extra, c);
        ASSERT_STREQ(s.chars(), cmp.c_str());
        ASSERT_EQ(s.len(), cmp.size());
    }
}

TEST_F(test_str, replace4)
{
    s = line;
    cmp = std::string(s.chars());

    s.replace(5, 4, 2, 'b');
    cmp.replace(5, 4, 2, 'b');

    ASSERT_STREQ(s.chars(), cmp.c_str());
    ASSERT_EQ(s.len(), cmp.size());
}

TEST_F(test_str, replace5)
{
    s = "aaaaa";
    cmp = "aaaaa";

    s.replace(3, 10, 8, 'b');
    cmp.replace(3, 10, 8, 'b');

    ASSERT_STREQ(s.chars(), cmp.c_str());
    ASSERT_EQ(s.len(), cmp.size());
}

TEST_F(test_str, replace6)
{
    s = str(line);
    cmp = std::string(line);
    auto dis = std::uniform_int_distribution<size_t>{1, 10};
    auto extra_count_dis = dis;
    auto rand_char = std::uniform_int_distribution<char>{'a', 'z'};
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        auto cnt = dis(mt);
        auto extra = extra_count_dis(mt);
        auto c = rand_char(mt);

        s.replace(i, cnt + extra, cnt, c);
        cmp.replace(i, cnt + extra, cnt, c);
        ASSERT_STREQ(s.chars(), cmp.c_str());
        ASSERT_EQ(s.len(), cmp.size());
    }
}

TEST_F(test_str, replace7)
{
    auto cmp = std::string("hello\tworld\t\t!");
    s = cmp.c_str();

    for (size_t i = 0; i < s.len(); ++i) {
        if (s[i] == '\t')
            s.replace(i, 1, 8, ' ');
        if (cmp[i] == '\t')
            cmp.replace(i, 1, 8, ' ');
    }

    ASSERT_STREQ(s.chars(), cmp.c_str());
    ASSERT_EQ(s.len(), cmp.size());
}
