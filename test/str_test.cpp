#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <gtest/gtest.h>
#include <cstring>
#include <iterator>
#include <random>
#include <string>
#include <vector>

#include "../src/str.hpp"

using std::begin, std::end;

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

    void SetUp() override
    {
        s = line;
        stls = line;
        mt.seed(std::random_device{}());

        ASSERT_STREQ(s.c_str(), line);
        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }

    str gen_str(char from, char to, size_t size)
    {
        auto ret = str();
        ret.resize(size);
        auto rand_c = std::uniform_int_distribution<char>(from, to);
        for (size_t i = 0; i < ret.size(); ++i)
            ret[i] = rand_c(mt);

        return ret;
    }
};

TEST_F(str_test, default_ctor)
{
    auto s = str();
    auto stls = std::string();
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
    ASSERT_EQ(s.front(), 0);
    ASSERT_EQ(s.back(), 0);
    ASSERT_EQ(s.front(), stls.front());
    ASSERT_EQ(s.back(), stls.back());
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

TEST_F(str_test, move_ctor_rand)
{
    auto generate_random_string = [](size_t size) {
        auto rand_c = std::uniform_int_distribution<char>('!', '~');
        auto mt = std::mt19937{};
        auto string = std::string();
        string.reserve(size);
        for (size_t i = 0; i < size; ++i)
            string.push_back(rand_c(mt));

        return string;
    };
    auto rand_size = std::uniform_int_distribution<size_t>(0, 50);

    for (auto i = 0; i < 1000; ++i) {
        auto s1 = str();
        auto size = rand_size(mt);
        auto rand_stls = generate_random_string(size);
        auto rand_str = str(rand_stls.c_str());
        ASSERT_STREQ(rand_str.c_str(), rand_stls.c_str());
        ASSERT_EQ(rand_str.size(), rand_stls.size());

        s1 = str(std::move(rand_str));

        ASSERT_STREQ(s1.c_str(), rand_stls.c_str());
        ASSERT_EQ(s1.size(), rand_stls.size());
    }
}

TEST_F(str_test, moved_reuse)
{
    auto generate_random_string = [](size_t size) {
        auto rand_c = std::uniform_int_distribution<char>('!', '~');
        auto mt = std::mt19937{};
        auto string = str();
        string.resize(size);
        for (size_t i = 0; i < size; ++i)
            string.push_back(rand_c(mt));

        return string;
    };

    auto vec = std::vector<str>();

    auto rand_size = std::uniform_int_distribution<size_t>(0, 50);
    auto i = 0;
    for (auto line = generate_random_string(rand_size(mt)); i++ < 1000;
            line = str(generate_random_string(rand_size(mt)))) {
        vec.push_back(std::move(line));
    }
}

TEST_F(str_test, iter_ctor1)
{
    auto s1 = str(begin(s), end(s));

    ASSERT_STREQ(s.c_str(), s1.c_str());
    ASSERT_EQ(s.size(), s1.size());
}

TEST_F(str_test, iter_ctor2)
{
    auto rand_first = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size() - 1));

    for (size_t i = 0; i < 10000; ++i) {
        auto first_idx = rand_first(mt);
        auto rand_last = std::uniform_int_distribution<ptrdiff_t>(first_idx,
                static_cast<ptrdiff_t>(s.size() - 1));
        auto last_idx = rand_last(mt);

        auto s1 = str(begin(s) + first_idx, begin(s) + last_idx);
        auto stls1 = std::string(begin(stls) + first_idx, begin(stls) + last_idx);

        ASSERT_STREQ(s1.c_str(), stls1.c_str());
        ASSERT_EQ(s1.size(), stls1.size());
    }
}

TEST_F(str_test, copy_swap_both_sbo)
{
    const auto* small_str = "hello, world!";
    auto small_str_size = std::strlen(small_str);

    auto s1 = str(small_str);
    ASSERT_STREQ(s1.c_str(), small_str);
    ASSERT_EQ(s1.size(), small_str_size);

    auto s2 = str();
    s2 = s1;
    ASSERT_STREQ(s2.c_str(), small_str);
    ASSERT_EQ(s2.size(), small_str_size);
}

TEST_F(str_test, copy_swap_left_sbo)
{
    auto sbo = str();
    auto dyn = str(line);

    sbo = dyn;
    ASSERT_STREQ(sbo.c_str(), dyn.c_str());
    ASSERT_EQ(sbo.size(), dyn.size());
}

TEST_F(str_test, copy_swap_right_sbo)
{
    auto dyn = str(line);
    const auto* small_str = "aaaaa";
    auto len = std::strlen(small_str);

    dyn = small_str;
    ASSERT_STREQ(dyn.c_str(), small_str);
    ASSERT_EQ(dyn.size(), len);
}

TEST_F(str_test, copy_swap_both_dyn)
{
    auto dyn1 = str(line);
    auto dyn2 = str("need to get beyond 15 character");

    dyn1 = dyn2;
    ASSERT_STREQ(dyn1.c_str(), dyn2.c_str());
    ASSERT_EQ(dyn1.size(), dyn2.size());
}

TEST_F(str_test, copy_swap2)
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

TEST_F(str_test, copy_swap3)
{
    auto tmp = str(line);
    s = std::move(tmp);

    ASSERT_STREQ(s.c_str(), line);
    ASSERT_EQ(s.size(), std::strlen(line));
}

TEST_F(str_test, copy_swap_random)
{
    auto generate_random_string = [](size_t size) {
        auto rand_c = std::uniform_int_distribution<char>('!', '~');
        auto mt = std::mt19937{};
        auto string = std::string();
        string.reserve(size);
        for (size_t i = 0; i < size; ++i)
            string.push_back(rand_c(mt));

        return string;
    };

    auto rand_size = std::uniform_int_distribution<size_t>(0, 50);

    for (auto i = 0; i < 1000; ++i) {
        auto s1 = str();
        auto size = rand_size(mt);
        auto rand_stls = generate_random_string(size);
        auto rand_str = str(rand_stls.c_str());
        ASSERT_STREQ(rand_str.c_str(), rand_stls.c_str());
        ASSERT_EQ(rand_str.size(), rand_stls.size());

        s1 = str(std::move(rand_str));

        ASSERT_STREQ(s1.c_str(), rand_stls.c_str());
        ASSERT_EQ(s1.size(), rand_stls.size());
    }
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

TEST_F(str_test, pop_back)
{
    auto generate_random_string = [](size_t size) {
        auto rand_c = std::uniform_int_distribution<char>('!', '~');
        auto mt = std::mt19937{};
        auto string = std::string();
        string.reserve(size);
        for (size_t i = 0; i < size; ++i)
            string.push_back(rand_c(mt));

        return string;
    };

    auto rand_size = std::uniform_int_distribution<size_t>(0, 50);
    for (auto i = 0; i < 100; ++i) {
        stls = generate_random_string(rand_size(mt));
        auto s1 = str(stls.c_str());

        while (!s1.empty()) {
            stls.pop_back();
            s1.pop_back();
            ASSERT_STREQ(s1.c_str(), stls.c_str());
            ASSERT_EQ(s1.size(), stls.size());
        }
        ASSERT_STREQ(s1.c_str(), stls.c_str());
        ASSERT_EQ(s1.size(), stls.size());
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

TEST_F(str_test, append_iter1)
{
    auto buf = str();
    buf.append(begin(s), end(s));

    ASSERT_STREQ(buf.c_str(), s.c_str());
}

TEST_F(str_test, append_iter2)
{
    std::copy(begin(s), end(s), begin(stls));

    ASSERT_EQ(stls.size(), s.size());

    auto buf = str();
    buf.append(begin(stls), end(stls));
    ASSERT_STREQ(buf.c_str(), s.c_str());
    ASSERT_EQ(buf.size(), s.size());
}

TEST_F(str_test, append_iter3)
{
    auto buf = std::string();
    ASSERT_STREQ(s.c_str(), stls.c_str());
    buf.append(begin(s), end(s));

    ASSERT_STREQ(buf.c_str(), stls.c_str());
    ASSERT_EQ(buf.size(), stls.size());
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
    auto orig_size = s.size();
    s.push_back('\n');
    s.remove_newline();

    ASSERT_EQ(s.size(), orig_size);
}

TEST_F(str_test, remove_newline2)
{
    auto blank_str = str("\n\n\n\r\r\r\n\r\n\r\r");
    blank_str.remove_newline();

    ASSERT_EQ(blank_str.size(), 0);
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
    stls = std::string(s.c_str());

    s.replace(5, 4, 2, 'b');
    stls.replace(5, 4, 2, 'b');

    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, replace5)
{
    const auto* buf = "aaaaa";
    s = buf;
    stls = buf;
    ASSERT_STREQ(s.c_str(), buf);
    ASSERT_EQ(s.size(), std::strlen(buf));

    return;
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
    return;
    stls = std::string("hello\tworld\t\t!");
    s = "hello\tworkd\t\t!";
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());

    return;

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

TEST_F(str_test, erase_iter1)
{
    while (s.size())
        s.erase(std::begin(s));
    ASSERT_STREQ(s.c_str(), "");
}

using std::begin;

TEST_F(str_test, erase_iter2)
{
    while (stls.size()) {
        auto rand_idx = std::uniform_int_distribution<ptrdiff_t>(0,
                static_cast<ptrdiff_t>(stls.size() - 1));
        auto idx = rand_idx(mt);

        s.erase(begin(s) + idx);
        stls.erase(begin(stls) + idx);
        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }
}

TEST_F(str_test, erase_iter3)
{
    const size_t MAX_SIZE = 100;
    auto generate_random_string = []() {
        auto rand_c = std::uniform_int_distribution<char>('!', '~');
        auto mt = std::mt19937{};
        auto string = std::string();
        string.reserve(MAX_SIZE);
        for (size_t i = 0; i < string.capacity(); ++i)
            string.push_back(rand_c(mt));

        return string;
    };

    auto rand_first = std::uniform_int_distribution<ptrdiff_t>(0, MAX_SIZE - 1);
    for (size_t i = 0; i < 1000; ++i) {
        stls = generate_random_string();
        s = stls.c_str();
        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());

        auto first_idx = rand_first(mt);
        auto rand_last = std::uniform_int_distribution<ptrdiff_t>(first_idx, MAX_SIZE);
        auto last_idx = rand_last(mt);

        s.erase(begin(s) + first_idx, begin(s) + last_idx);
        stls.erase(begin(stls) + first_idx, begin(stls) + last_idx);

        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }
}

TEST_F(str_test, find1)
{
    for (char c = 'a'; c <= 'z'; ++c) {
        for (auto i = 0; i < 100; ++i) {
            auto s = gen_str('a', c, 50);
            auto stls = std::string(s.c_str());

            for (auto j = 0; j < 100; ++j) {
                auto rand_begin = std::uniform_int_distribution<ptrdiff_t>(0,
                        static_cast<ptrdiff_t>(s.size() - 1));
                auto begin_idx = rand_begin(mt);
                auto rand_end = std::uniform_int_distribution<ptrdiff_t>(begin_idx,
                        static_cast<ptrdiff_t>(s.size()));
                auto end_idx = rand_end(mt);

                auto substr = str(begin(s) + begin_idx, begin(s) + end_idx);
                ASSERT_EQ(s.find(substr), stls.find(substr.c_str()));
            }
        }
    }
}

TEST_F(str_test, find2)
{
    auto rand_haystack_size = std::uniform_int_distribution<size_t>(0, 120);
    auto rand_needle_size = std::uniform_int_distribution<size_t>(0, 100);
    for (char c = '!'; c <= '~'; ++c) {
        for (auto i = 0; i < 100; ++i) {
            auto haystack = gen_str('!', c, rand_haystack_size(mt));
            auto stls_haystack = std::string(haystack.c_str());

            for (auto j = 0; j < 20; ++j) {
                auto needle = gen_str('!', c, rand_needle_size(mt));
                auto stls_needle = std::string(needle.c_str());
                ASSERT_EQ(haystack.find(needle), stls_haystack.find(stls_needle))
                    << "haystack size: " << haystack.size() << '\n'
                    << "needle size: " << needle.size() << '\n';
                ASSERT_EQ(needle.find(haystack), stls_needle.find(stls_haystack))
                    << "haystack size: " << haystack.size() << '\n'
                    << "needle size: " << needle.size() << '\n';
            }
        }
    }
}
