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

using std::cbegin, std::cend;

class str_const_iter_test : public ::testing::Test
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
                       "agcendi aliquid disccendique"
                       "causa prope contra naturam"
                       "vígillas suscipere soleamus.";
    std::mt19937 mt{};

    void func(str::const_iterator const_it)
    {
    }

    void SetUp() override
    {
        s = line;
        cmp = line;

        ASSERT_STREQ(s.c_str(), line);
        ASSERT_STREQ(s.c_str(), cmp.c_str());
        ASSERT_EQ(s.size(), cmp.size());
    }
};

TEST_F(str_const_iter_test, range_based_for_read)
{
    size_t i = 0;
    for (auto c : s)
        ASSERT_EQ(c, cmp[i++]);
}

TEST_F(str_const_iter_test, range_based_for_write)
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

TEST_F(str_const_iter_test, iter_copy)
{
    size_t i = 0;
    for (auto it = cbegin(s); it != cend(s); ++it) {
        const auto cp = it;
        ASSERT_EQ(*it, s[i++]);
        ASSERT_EQ(*cp, *it);
    }
}

TEST_F(str_const_iter_test, ordering_less)
{
    auto it = cbegin(s);
    auto rand_diff = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    for (size_t i = 0; i < 1000; ++i) {
        auto lhs = rand_diff(mt);
        auto rhs = rand_diff(mt);
        auto res = lhs < rhs;
        ASSERT_EQ(it + lhs < it + rhs, res);
    }
}

TEST_F(str_const_iter_test, ordering_less_eq)
{
    auto it = cbegin(s);
    auto rand_diff = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    for (size_t i = 0; i < 1000; ++i) {
        auto lhs = rand_diff(mt);
        auto rhs = rand_diff(mt);
        auto res = lhs <= rhs;
        ASSERT_EQ(it + lhs <= it + rhs, res);
    }
}

TEST_F(str_const_iter_test, ordering_eq)
{
    auto it = cbegin(s);
    auto rand_diff = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    for (size_t i = 0; i < 1000; ++i) {
        auto lhs = rand_diff(mt);
        auto rhs = rand_diff(mt);
        auto res = lhs == rhs;
        ASSERT_EQ(it + lhs == it + rhs, res);
    }
}

TEST_F(str_const_iter_test, ordering_eq_greater)
{
    auto it = cbegin(s);
    auto rand_diff = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    for (size_t i = 0; i < 1000; ++i) {
        auto lhs = rand_diff(mt);
        auto rhs = rand_diff(mt);
        auto res = lhs >= rhs;
        ASSERT_EQ(it + lhs >= it + rhs, res);
    }
}

TEST_F(str_const_iter_test, greater)
{
    auto it = cbegin(s);
    auto rand_diff = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    for (size_t i = 0; i < 1000; ++i) {
        auto lhs = rand_diff(mt);
        auto rhs = rand_diff(mt);
        auto res = lhs > rhs;
        ASSERT_EQ(it + lhs > it + rhs, res);
    }
}

TEST_F(str_const_iter_test, random_access_read)
{
    auto it = std::cbegin(s);
    auto cmp_it = std::cbegin(cmp);
    auto rand = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);

    for (size_t i = 0; i < 1000; ++i) {
        auto idx = rand(mt);
        ASSERT_EQ(*(it + idx), *(cmp_it + idx));
        ASSERT_EQ(*(it + idx), s[static_cast<size_t>(idx)]);
    }
}

/*
TEST_F(str_const_iter_test, stl_find)
{
    auto rand_c = std::uniform_int_distribution<char>('a', 'z');

    for (auto i = 0; i < 1000; ++i) {
        auto c = rand_c(mt);

        auto res1 = std::find(cbegin(s), cend(s), c) == cend(s);
        auto res2 = std::find(cbegin(cmp), cend(cmp), c) == cend(cmp);
        ASSERT_EQ(res1, res2);
    }
}
*/

TEST_F(str_const_iter_test, stl_remove)
{
    // TODO add member func str.erase(it) and test remove-erase idiom
}

TEST_F(str_const_iter_test, stl_unique)
{
    // TODO add member func str.erase(it) and test remove-erase idiom
}

TEST_F(str_const_iter_test, stl_count)
{
    for (size_t i = 0; i < s.size(); ++i) {
        auto cnt1 = std::count(cbegin(s), cend(s), s[i]);
        auto cnt2 = std::count(cbegin(cmp), cend(cmp), cmp[i]);
        ASSERT_EQ(cnt1, cnt2);
    }

    auto it1 = cbegin(s);
    auto it2 = cbegin(s);
}

/*
 * FIXME somehoe std::advance and std::distance don't work on const_iterator
TEST_F(str_const_iter_test, stl_distance)
{
    auto rand_idx = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<long>(s.size()));
    for (size_t i = 0; i < 10000; ++i) {
        auto idx1 = rand_idx(mt);
        auto idx2 = rand_idx(mt);

        auto it1 = cbegin(s);
        auto it2 = cbegin(s);
        std::advance(it1, idx1);
        std::advance(it2, idx2);

        auto dist = std::distance(it1, it2);
        ASSERT_EQ(dist, idx2 - idx1);
    }
}
*/