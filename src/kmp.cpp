#include "kmp.hpp"
#include <cstddef>
#include <string>
#include <vector>

std::vector<size_t> gen_lps(const std::string& needle)
{
    auto lps = std::vector<size_t>(needle.size(), 0);
    size_t i = 0, j = 1;
    while (j < needle.size()) {
        if (needle[i] == needle[j]) {
            lps[j++] = ++i;
        } else {
            if (!i)
                ++j;
            else
                i = lps[i - 1];
        }
    }
    return lps;
}

size_t kmp(const std::string& haystack, const std::string& needle)
{
    auto lps = gen_lps(needle);

    for (size_t i = 0, j = 0; i < haystack.size();) {
        for (; j < needle.size() && haystack[i + j] == needle[j]; ++j);
        if (j == needle.size())
            return i;

        auto skip = (j) ? j - lps[j - 1] : 1;   // 1: get lps from the matched
                                                //    substr
        i += skip;                              // 2: skip the lps
        j = (skip != 1) ? lps[j - 1] : 0;       // 3: skip the already matched
                                                //    prefix too
    }

    return static_cast<size_t>(-1);
}
