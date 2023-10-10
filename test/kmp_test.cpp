#include "../src/kmp.hpp"

#include <cstddef>
#include <gtest/gtest.h>
#include <random>
#include <string>
#include <fmt/ranges.h>

static std::mt19937 mt;

static std::vector<size_t> lps_correct(std::string txt)
{
    auto lps = std::vector<size_t>(txt.size());

    lps[0] = 0;
    size_t len = 0;
    size_t i = 1;
    while (i < txt.length()) {
        if (txt[i] == txt[len]) {
            len++;
            lps[i] = len;
            i++;
            continue;
        } else {
            if (len == 0) {
                lps[i] = 0;
                i++;
                continue;
            } else {
                len = lps[len - 1];
                continue;
            }
        }
    }

    return lps;
}

static std::string gen_str(size_t size, char from, char to)
{
    auto s = std::string();
    s.resize(size);
    auto rand_c = std::uniform_int_distribution<char>(from, to);
    for (size_t i = 0; i < s.size(); ++i)
        s[i] = rand_c(mt);

    return s;
}

TEST(kmp_test, lsp1)
{
    GTEST_SKIP();

    for (char c = 'a'; c <= 'z'; ++c) {
        for (auto i = 0; i < 100; ++i) {
            auto s = gen_str(50, 'a', c);
            fmt::println("{}", s);

            auto res = gen_lps(s);
            auto correct = lps_correct(s);
            fmt::println("{}", fmt::join(res, ""));

            ASSERT_EQ(res, correct);
        }
    }
}

TEST(kmp_test, kmp1)
{
    for (char c = 'a'; c <= 'z'; ++c) {
        for (auto i = 0; i < 100; ++i) {
            auto s = gen_str(50, 'a', c);

            for (auto j = 0; j < 20; ++j) {
                auto rand_begin = std::uniform_int_distribution<ptrdiff_t>(0,
                        static_cast<ptrdiff_t>(s.size() - 1));
                auto begin_idx = rand_begin(mt);
                auto rand_end = std::uniform_int_distribution<ptrdiff_t>(begin_idx,
                        static_cast<ptrdiff_t>(s.size()));
                auto end_idx = rand_end(mt);

                auto substr = std::string(begin(s) + begin_idx, begin(s) + end_idx);
                // fmt::println("s\t: {}", s);
                // fmt::println("substr\t: {}", substr);
                ASSERT_EQ(kmp(s, substr), s.find(substr));
            }
        }
    }
}
