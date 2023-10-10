#pragma once

#include <cstddef>
#include <vector>
#include <string>

std::vector<size_t> gen_lps(const std::string& needle);
size_t kmp(const std::string& haystack, const std::string& needle);
