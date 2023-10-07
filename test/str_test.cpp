#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fmt/core.h>
#include <gtest/gtest.h>
#include <cstring>
#include <iterator>
#include <random>
#include <string>

#include "../src/str.hpp"

class str_test : public ::testing::Test
{
protected:
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
    str s;
    std::string stls;
    std::mt19937 mt{};
};

TEST_F(str_test, default_ctor)
{
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
    ASSERT_EQ(s.capacity(), stls.capacity());

    ASSERT_EQ(s.front(), 0);
    ASSERT_EQ(s.back(), 0);
}

TEST_F(str_test, char_ptr_ctor)
{
    auto s = str(line);
    auto stls = std::string(line);
    ASSERT_STREQ(s.c_str(), line);
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_NE(reinterpret_cast<std::uintptr_t>(s.c_str()),
            reinterpret_cast<std::uintptr_t>(line));
    ASSERT_EQ(s.size(), std::strlen(line));
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, copy_ctor)
{
    auto s2 = str(line);
    auto s3 = s2;

    ASSERT_STREQ(s3.c_str(), s2.c_str());
    ASSERT_EQ(s3.size(), s2.size());
    ASSERT_NE(s3.c_str(), s2.c_str());
}

TEST_F(str_test, move_ctor)
{
    auto s1 = str();
    auto s2 = str(line);
    auto addr = reinterpret_cast<intptr_t>(s2.c_str());

    s1 = std::move(s2);

    ASSERT_EQ(addr, reinterpret_cast<intptr_t>(s1.c_str()));
    ASSERT_STREQ(s1.c_str(), line);
    ASSERT_EQ(s1.size(), std::strlen(line));
}

TEST_F(str_test, copy_assignment)
{
    auto tmp = str(line);
    s = tmp;

    ASSERT_STREQ(s.c_str(), tmp.c_str());
    ASSERT_NE(s.c_str(), tmp.c_str());
    ASSERT_EQ(s.size(), tmp.size());

    s = str();
    ASSERT_EQ(s.size(), 0);
    ASSERT_EQ(s.capacity(), 15);
    ASSERT_STREQ(s.c_str(), "");
}

TEST_F(str_test, move_assignment)
{
    auto tmp = str(line);
    s = std::move(tmp);

    ASSERT_STREQ(s.c_str(), line);
    ASSERT_EQ(s.size(), std::strlen(line));
}

TEST_F(str_test, Front)
{
    s = str(line);
    ASSERT_EQ(s.front(), line[0]);
}

TEST_F(str_test, back_and_push_back)
{
    s = str();
    stls = std::string();
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        s.push_back(line[i]);
        stls.push_back(line[i]);

        ASSERT_EQ(s.back(), line[i]);
        ASSERT_EQ(s.size(), stls.size());
        ASSERT_STREQ(s.c_str(), stls.c_str());
    }
}

TEST_F(str_test, append1)
{
    auto len = std::strlen(line);
    auto rand_idx = std::uniform_int_distribution<size_t>(0, len - 1);
    auto rand_cnt = std::uniform_int_distribution<size_t>(0, 100);
    for (size_t i = 0; i < 100; ++i) {
        auto idx = rand_idx(mt);
        auto cnt = rand_cnt(mt);
        s.append(cnt, line[idx]);
        stls.append(cnt, line[idx]);

        ASSERT_EQ(s.size(), stls.size());
        ASSERT_STREQ(s.c_str(), stls.c_str());
    }
}

TEST_F(str_test, append2)
{
    s.append(str());
    stls.append(std::string());
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());

    for (size_t i = 0; i < 5; ++i) {
        s.append(line);
        stls.append(line);

        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }

    s.clear();
    stls.clear();
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, append3)
{
    auto rand = std::uniform_int_distribution<size_t>(0, 100);
    for (size_t i = 0; i < 100; ++i) {
        auto cnt = rand(mt);
        s.append(line, cnt);
        stls.append(line, cnt);

        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }
}

TEST_F(str_test, insert1)
{
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        s.insert(i, 1, line[i]);
        stls.insert(i, 1, line[i]);
    }

    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());

    s.clear();
    stls.clear();
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, insert2)
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
        stls.insert(insert_idx, cnt, line[line_idx]);
        ASSERT_STREQ(s.c_str(), stls.c_str());
    }

    s.clear();
    stls.clear();
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, insert_str)
{
    const auto line = str{ "Lorem ipsum dolor sit amet, consectetur:" };

    for (size_t i = 0; i < 50; ++i) {
        auto insert_idx_range = std::uniform_int_distribution<size_t>(0, i);
        auto insert_idx = insert_idx_range(mt);

        s.insert(insert_idx, line);
        stls.insert(insert_idx, line.c_str());

        ASSERT_STREQ(s.c_str(), stls.c_str());
    }

    s.clear();
    stls.clear();
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, resize1)
{
    s = line;
    stls = line;
    auto rand = std::uniform_int_distribution<size_t>(0, 100);
    for (size_t i = 0; i < 50; ++i) {
        auto size = rand(mt);
        s.resize(size);
        stls.resize(size);
        ASSERT_EQ(s.size(), stls.size());
        ASSERT_STREQ(s.c_str(), stls.c_str());
    }
}

TEST_F(str_test, resize2)
{
    s = line;
    stls = line;
    auto rand = std::uniform_int_distribution<size_t>(0, 100);
    auto rand_alpha = std::uniform_int_distribution<char>('a', 'z');
    for (size_t i = 0; i < 50; ++i) {
        auto size = rand(mt);
        auto c = rand_alpha(mt);
        s.resize(size, c);
        stls.resize(size, c);
        ASSERT_EQ(s.size(), stls.size());
        ASSERT_STREQ(s.c_str(), stls.c_str());
    }
}

TEST_F(str_test, remove_newline)
{
    s.push_back('\n');
    s.remove_newline();

    ASSERT_EQ(s.size(), 0);
}

TEST_F(str_test, replace1)
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
    stls = std::string(line);
    ASSERT_EQ(s.size(), stls.size());

    auto dis = std::uniform_int_distribution<size_t>{1, 10};
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        auto cnt = dis(mt);
        s.replace(i, cnt, cnt, 'c');
        stls.replace(i, cnt, cnt, 'c');
        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }
}

TEST_F(str_test, replace2)
{
    s = line;
    stls = std::string(s.c_str());

    s.replace(2, 2, 4, 'b');
    stls.replace(2, 2, 4, 'b');

    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, replace3)
{
    s = str(line);
    stls = std::string(line);
    auto dis = std::uniform_int_distribution<size_t>{1, 10};
    auto extra_count_dis = dis;
    auto rand_char = std::uniform_int_distribution<char>{'a', 'z'};
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        auto cnt = dis(mt);
        auto extra = extra_count_dis(mt);
        char c = rand_char(mt);

        s.replace(i, cnt, cnt + extra, c);
        stls.replace(i, cnt, cnt + extra, c);
        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }
}

TEST_F(str_test, replace4)
{
    s = line;
    stls = std::string(s.c_str());

    s.replace(5, 4, 2, 'b');
    stls.replace(5, 4, 2, 'b');

    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, replace5)
{
    s = "aaaaa";
    stls = "aaaaa";

    s.replace(3, 10, 8, 'b');
    stls.replace(3, 10, 8, 'b');

    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, replace6)
{
    s = str(line);
    stls = std::string(line);
    auto dis = std::uniform_int_distribution<size_t>{1, 10};
    auto extra_count_dis = dis;
    auto rand_char = std::uniform_int_distribution<char>{'a', 'z'};
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        auto cnt = dis(mt);
        auto extra = extra_count_dis(mt);
        auto c = rand_char(mt);

        s.replace(i, cnt + extra, cnt, c);
        stls.replace(i, cnt + extra, cnt, c);
        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }
}

TEST_F(str_test, replace7)
{
    stls = std::string("hello\tworld\t\t!");
    s = stls.c_str();

    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\t')
            s.replace(i, 1, 8, ' ');
        if (stls[i] == '\t')
            stls.replace(i, 1, 8, ' ');
    }

    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, erase1)
{
    s = line;
    stls = line;

    auto rand_erase_cnt = std::uniform_int_distribution<std::size_t>(0, 3);
    while (stls.size()) {
        auto rand_idx = std::uniform_int_distribution<size_t>(0, stls.size());
        auto idx = rand_idx(mt);
        auto cnt = rand_erase_cnt(mt);
        s.erase(idx, cnt);
        stls.erase(idx, cnt);
        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }
}
