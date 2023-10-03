#include <fmt/core.h>
#include <gtest/gtest.h>
#include <cstring>
#include <random>
#include <string>

#include "../src/str.hpp"

class StrTest : public ::testing::Test
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

TEST_F(StrTest, DefaultCtor)
{
    ASSERT_EQ(s.chars(), nullptr);
}

TEST_F(StrTest, CharPtrCtor)
{
    auto s = str(line);
    ASSERT_STREQ(s.chars(), line);
    ASSERT_NE(s.chars(), line);
    ASSERT_EQ(s.len(), std::strlen(line));
}

TEST_F(StrTest, MovedCharPtrCtor)
{
    auto* ptr = new char[std::strlen(line) + 1]{};
    std::strcpy(ptr, line);
    auto addr = reinterpret_cast<intptr_t>(ptr);

    auto s = str(std::move(ptr));

    ASSERT_EQ(reinterpret_cast<intptr_t>(s.chars()), addr);
    ASSERT_STREQ(s.chars(), line);
}

TEST_F(StrTest, CopyCtor)
{
    auto s1 = str();
    auto s2 = str(line);
    s1 = s2;

    ASSERT_STREQ(s1.chars(), s2.chars());
    ASSERT_EQ(s1.len(), s2.len());
    ASSERT_NE(s1.chars(), s2.chars());
}

TEST_F(StrTest, MoveCtor)
{
    auto s1 = str();
    auto s2 = str(line);
    auto addr = reinterpret_cast<intptr_t>(s2.chars());

    s1 = std::move(s2);

    ASSERT_EQ(addr, reinterpret_cast<intptr_t>(s1.chars()));
    ASSERT_STREQ(s1.chars(), line);
    ASSERT_EQ(s1.len(), std::strlen(line));
}

TEST_F(StrTest, OperatorEqual)
{
    auto tmp = str(line);
    s = tmp;

    ASSERT_STREQ(s.chars(), tmp.chars());
    ASSERT_NE(s.chars(), tmp.chars());
    ASSERT_EQ(s.len(), tmp.len());
}

TEST_F(StrTest, OperatorEqualMove)
{
    auto tmp = str(line);
    s = std::move(tmp);
}

TEST_F(StrTest, Front)
{
    s = str(line);
    ASSERT_EQ(s.front(), line[0]);
}

TEST_F(StrTest, BackAndPushBack)
{
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        s.push_back(line[i]);
        ASSERT_EQ(s.back(), line[i]);
    }
}

TEST_F(StrTest, Append)
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
}

TEST_F(StrTest, Insert1)
{
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        s.insert(i, 1, line[i]);
        cmp.insert(i, 1, line[i]);
    }

    ASSERT_STREQ(s.chars(), cmp.c_str());

    s.clear();
    cmp.clear();
    ASSERT_STREQ(s.chars(), cmp.c_str());
}

TEST_F(StrTest, Insert2)
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
}

TEST_F(StrTest, InsertStr)
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
}

TEST_F(StrTest, RmNewline)
{
    const auto* buf = "hello, world";
    s = buf;
    s.push_back('\n');
    s.remove_newline();

    ASSERT_STREQ(s.chars(), buf);
}

TEST_F(StrTest, ReplaceCntEqualCnt2)
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

    auto dis = std::uniform_int_distribution<size_t>{1, 10};
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        auto cnt = dis(mt);
        s.replace(i, cnt, cnt, 'c');
        cmp.replace(i, cnt, cnt, 'c');
        ASSERT_STREQ(s.chars(), cmp.c_str());
    }
}

TEST_F(StrTest, ReplaceCntEqualCnt2Simple)
{
    s = line;
    cmp = std::string(s.chars());

    s.replace(2, 2, 4, 'b');
    cmp.replace(2, 2, 4, 'b');

    ASSERT_STREQ(s.chars(), cmp.c_str());
}

TEST_F(StrTest, ReplaceCntLessCnt2)
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
    }
}

TEST_F(StrTest, ReplaceCntGreaterCnt2Simple)
{
    s = line;
    cmp = std::string(s.chars());

    s.replace(5, 4, 2, 'b');
    cmp.replace(5, 4, 2, 'b');

    ASSERT_STREQ(s.chars(), cmp.c_str());
}

TEST_F(StrTest, Replace2)
{
    s = "aaaaa";
    cmp = "aaaaa";

    s.replace(3, 10, 8, 'b');
    cmp.replace(3, 10, 8, 'b');

    ASSERT_STREQ(s.chars(), cmp.c_str());
}

TEST_F(StrTest, ReplaceCntGreaterCnt2)
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
    }
}
