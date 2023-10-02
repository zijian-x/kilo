#include <gtest/gtest.h>
#include <cstring>
#include <random>
#include <string>

#include "../src/str.hpp"

class StrTest : public ::testing::Test
{
protected:
    str s;
};

TEST_F(StrTest, DefaultCtor)
{
    ASSERT_EQ(s.chars(), nullptr);
}

TEST_F(StrTest, CharPtrCtor)
{
    const auto* line = "Hello, world";
    auto s = str(line);
    ASSERT_STREQ(s.chars(), line);
    ASSERT_NE(s.chars(), line);
    ASSERT_EQ(s.len(), std::strlen(line));
}

TEST_F(StrTest, MovedCharPtrCtor)
{
    const auto* buf = "hello, world";
    auto* ptr = new char[std::strlen(buf) + 1]{};
    std::strcpy(ptr, buf);
    auto addr = reinterpret_cast<intptr_t>(ptr);

    auto s = str(std::move(ptr));

    ASSERT_EQ(reinterpret_cast<intptr_t>(s.chars()), addr);
    ASSERT_STREQ(s.chars(), buf);
}

TEST_F(StrTest, CopyCtor)
{
    auto s1 = str();
    auto s2 = str("hello, world");
    s1 = s2;

    ASSERT_STREQ(s1.chars(), s2.chars());
    ASSERT_EQ(s1.len(), s2.len());
    ASSERT_NE(s1.chars(), s2.chars());
}

TEST_F(StrTest, MoveCtor)
{
    const auto* buf = "hello, world";
    auto s1 = str();
    auto s2 = str(buf);
    auto addr = reinterpret_cast<intptr_t>(s2.chars());

    s1 = std::move(s2);

    ASSERT_EQ(addr, reinterpret_cast<intptr_t>(s1.chars()));
    ASSERT_STREQ(s1.chars(), buf);
    ASSERT_EQ(s1.len(), std::strlen(buf));
}

TEST_F(StrTest, OperatorEqual)
{
    auto tmp = str("Hello, world");
    s = tmp;

    ASSERT_STREQ(s.chars(), tmp.chars());
    ASSERT_NE(s.chars(), tmp.chars());
    ASSERT_EQ(s.len(), tmp.len());
}

TEST_F(StrTest, OperatorEqualMove)
{
    const auto* line = "Hello, world";
    auto tmp = str(line);
    s = std::move(tmp);
}

TEST_F(StrTest, MethodFront)
{
    s = str("hello, world");
    ASSERT_EQ(s.front(), 'h');
}

TEST_F(StrTest, MethodBackAndPushBack)
{
    const auto* buf = "hello, world!";
    for (size_t i = 0; i < std::strlen(buf); ++i) {
        s.push_back(buf[i]);
        ASSERT_EQ(s.back(), buf[i]);
    }
}

TEST_F(StrTest, MethodAppend)
{
    const auto* buf = "hello, world!";
    auto cmp = std::string();
    for (size_t i = 0; i < 5; ++i) {
        s.append(buf);
        cmp.append(buf);
    }

    ASSERT_STREQ(s.chars(), cmp.c_str());
    ASSERT_EQ(s.len(), cmp.size());

    s.clear();
    cmp.clear();
    ASSERT_STREQ(s.chars(), cmp.c_str());
}

TEST_F(StrTest, MethodInsert1)
{
    const auto buf = std::string("hello, world");
    auto cmp = std::string{};
    for (size_t i = 0; i < buf.size(); ++i) {
        s.insert(i, 1, buf[i]);
        cmp.insert(i, 1, buf[i]);
    }

    ASSERT_STREQ(s.chars(), cmp.c_str());

    s.clear();
    cmp.clear();
    ASSERT_STREQ(s.chars(), cmp.c_str());
}

TEST_F(StrTest, MethodInsert2)
{
    const auto line = std::string{
        "Lorem ipsum dolor sit amet, consectetur:"
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
            "vígillas suscipere soleamus."
    };

    auto cmp = std::string();
    auto mt = std::mt19937();
    auto line_idx_range = std::uniform_int_distribution<size_t>(0, line.size() - 1);
    auto count_range = std::uniform_int_distribution<size_t>(0, 20);

    for (size_t i = 0; i < line.size(); ++i) {
        auto insert_idx_range = std::uniform_int_distribution<size_t>(0, i);
        auto insert_idx = insert_idx_range(mt);
        auto line_idx = line_idx_range(mt);
        auto cnt = count_range(mt);

        s.insert(insert_idx, cnt, line[line_idx]);
        cmp.insert(insert_idx, cnt, line[line_idx]);
    }

    ASSERT_STREQ(s.chars(), cmp.c_str());

    s.clear();
    cmp.clear();
    ASSERT_STREQ(s.chars(), cmp.c_str());
}

TEST_F(StrTest, MethodInsertStr)
{
    const auto line = str{ "Lorem ipsum dolor sit amet, consectetur:" };

    auto cmp = std::string();
    auto mt = std::mt19937();

    for (size_t i = 0; i < 50; ++i) {
        auto insert_idx_range = std::uniform_int_distribution<size_t>(0, i);
        auto insert_idx = insert_idx_range(mt);

        s.insert(insert_idx, line);
        cmp.insert(insert_idx, line.chars());
    }

    ASSERT_STREQ(s.chars(), cmp.c_str());

    s.clear();
    cmp.clear();
    ASSERT_STREQ(s.chars(), cmp.c_str());
}

TEST_F(StrTest, MethodRmNewline)
{
    const auto* buf = "hello, world";
    s = buf;
    s.push_back('\n');
    s.remove_newline();

    ASSERT_STREQ(s.chars(), buf);
}
