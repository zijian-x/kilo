#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <fmt/core.h>
#include <gtest/gtest.h>
#include <iterator>
#include <random>
#include <unordered_set>

#include "../src/str.hpp"

class str_iter_test : public ::testing::Test
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

    void SetUp() override
    {
        s = line;
        cmp = line;

        ASSERT_STREQ(s.c_str(), line);
        ASSERT_STREQ(s.c_str(), cmp.c_str());
        ASSERT_EQ(s.size(), cmp.size());
    }
};

using std::begin, std::end;

TEST_F(str_iter_test, range_based_for_read)
{
    size_t i = 0;
    for (auto c : s)
        ASSERT_EQ(c, cmp[i++]);
}

TEST_F(str_iter_test, range_based_for_write)
{
    size_t i = 0;
    for (auto& c : s) {
        c = 'a';
        ASSERT_EQ(s[i++], 'a');
    }
    for (auto& c : cmp) {
        c = 'a';
    }

    ASSERT_STREQ(s.c_str(), cmp.c_str());
}

TEST_F(str_iter_test, iter_copy)
{
    size_t i = 0;
    for (auto it = begin(s); it != end(s); ++it) {
        const auto cp = it;
        ASSERT_EQ(*it, s[i++]);
        ASSERT_EQ(*cp, *it);
    }
}

TEST_F(str_iter_test, ordering_less)
{
    auto it = begin(s);
    auto rand_diff = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    for (size_t i = 0; i < 1000; ++i) {
        auto lhs = rand_diff(mt);
        auto rhs = rand_diff(mt);
        auto res = lhs < rhs;
        ASSERT_EQ(it + lhs < it + rhs, res);
    }
}

TEST_F(str_iter_test, ordering_less_eq)
{
    auto it = begin(s);
    auto rand_diff = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    for (size_t i = 0; i < 1000; ++i) {
        auto lhs = rand_diff(mt);
        auto rhs = rand_diff(mt);
        auto res = lhs <= rhs;
        ASSERT_EQ(it + lhs <= it + rhs, res);
    }
}

TEST_F(str_iter_test, ordering_eq)
{
    auto it = begin(s);
    auto rand_diff = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    for (size_t i = 0; i < 1000; ++i) {
        auto lhs = rand_diff(mt);
        auto rhs = rand_diff(mt);
        auto res = lhs == rhs;
        ASSERT_EQ(it + lhs == it + rhs, res);
    }
}

TEST_F(str_iter_test, ordering_eq_greater)
{
    auto it = begin(s);
    auto rand_diff = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    for (size_t i = 0; i < 1000; ++i) {
        auto lhs = rand_diff(mt);
        auto rhs = rand_diff(mt);
        auto res = lhs >= rhs;
        ASSERT_EQ(it + lhs >= it + rhs, res);
    }
}

TEST_F(str_iter_test, greater)
{
    auto it = begin(s);
    auto rand_diff = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    for (size_t i = 0; i < 1000; ++i) {
        auto lhs = rand_diff(mt);
        auto rhs = rand_diff(mt);
        auto res = lhs > rhs;
        ASSERT_EQ(it + lhs > it + rhs, res);
    }
}

TEST_F(str_iter_test, random_access_read)
{
    auto it = std::begin(s);
    auto cmp_it = std::begin(cmp);
    auto rand = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);

    for (size_t i = 0; i < 1000; ++i) {
        auto idx = rand(mt);
        ASSERT_EQ(*(it + idx), *(cmp_it + idx));
        ASSERT_EQ(*(it + idx), s[static_cast<size_t>(idx)]);
    }
}

TEST_F(str_iter_test, random_access_write)
{
    auto it = std::begin(s);
    auto cmp_it = std::begin(cmp);
    auto rand = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    auto rand_char = std::uniform_int_distribution<char>('a', 'z');

    for (size_t i = 0; i < 1000; ++i) {
        auto idx = rand(mt);
        auto c = rand_char(mt);
        *it = c;
        *cmp_it = c;
        ASSERT_EQ(*it, c);
        ASSERT_EQ(*(it + idx), *(cmp_it + idx));
        ASSERT_EQ(*(it + idx), s[static_cast<size_t>(idx)]);
    }
}

TEST_F(str_iter_test, stl_sort_default)
{
    std::sort(begin(s), end(s));
    std::sort(begin(cmp), end(cmp));

    ASSERT_STREQ(s.c_str(), cmp.c_str());
}

TEST_F(str_iter_test, stl_sort_greater_eq)
{
    std::sort(begin(s), end(s), std::greater_equal<char>{});
    std::sort(begin(cmp), end(cmp), std::greater_equal<char>{});

    ASSERT_STREQ(s.c_str(), cmp.c_str());
}

TEST_F(str_iter_test, stl_sort_greater)
{
    std::sort(begin(s), end(s), std::greater<char>{});
    std::sort(begin(cmp), end(cmp), std::greater<char>{});

    ASSERT_STREQ(s.c_str(), cmp.c_str());
}

TEST_F(str_iter_test, stl_copy)
{
    auto buf = str();
    auto cmp_buf = std::string();

    buf.resize(s.size());
    cmp_buf.resize(cmp.size());

    std::copy(begin(s), end(s), begin(buf));
    std::copy(begin(cmp), end(cmp), begin(cmp_buf));

    ASSERT_STREQ(buf.c_str(), cmp_buf.c_str());
}

TEST_F(str_iter_test, stl_transform)
{
    auto buf = str();
    buf.resize(s.size());
    auto cmp_buf = std::string();
    cmp_buf.resize(s.size());

    std::transform(begin(s), end(s), begin(buf), [](char c) { return std::toupper(c); });
    std::transform(begin(cmp), end(cmp), begin(cmp_buf), [](char c) { return std::toupper(c); });

    ASSERT_STREQ(buf.c_str(), cmp_buf.c_str());
}

TEST_F(str_iter_test, stl_find)
{
    auto rand_c = std::uniform_int_distribution<char>('a', 'z');

    for (auto i = 0; i < 1000; ++i) {
        auto c = rand_c(mt);

        auto res1 = std::find(begin(s), end(s), c) == end(s);
        auto res2 = std::find(begin(cmp), end(cmp), c) == end(cmp);
        ASSERT_EQ(res1, res2);
    }
}

TEST_F(str_iter_test, stl_remove)
{
    // TODO add member func str.erase(it) and test remove-erase idiom
}

TEST_F(str_iter_test, stl_unique)
{
    // TODO add member func str.erase(it) and test remove-erase idiom
}

TEST_F(str_iter_test, stl_reverse)
{
    std::reverse(begin(s), end(s));
    std::reverse(begin(cmp), end(cmp));

    ASSERT_STREQ(s.c_str(), cmp.c_str());
}

TEST_F(str_iter_test, stl_count)
{
    for (size_t i = 0; i < s.size(); ++i) {
        auto cnt1 = std::count(begin(s), end(s), s[i]);
        auto cnt2 = std::count(begin(cmp), end(cmp), cmp[i]);
        ASSERT_EQ(cnt1, cnt2);
    }
}

TEST_F(str_iter_test, stl_distance)
{
    auto rand_idx = std::uniform_int_distribution<size_t>(0, s.size());
    for (size_t i = 0; i < 10000; ++i) {
        auto idx1 = rand_idx(mt);
        auto idx2 = rand_idx(mt);

        auto it1 = begin(s);
        auto it2 = begin(s);
        std::advance(it1, idx1);
        std::advance(it2, idx2);

        auto dist = std::distance(it1, it2);
        ASSERT_EQ(dist, idx2 - idx1);
    }
}

TEST_F(str_iter_test, stl_fill)
{
    std::fill(begin(s), end(s), 'c');
    std::fill(begin(cmp), end(cmp), 'c');
    ASSERT_STREQ(s.c_str(), cmp.c_str());
}