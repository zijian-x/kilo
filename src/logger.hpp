#pragma once

#include "file_raii.hpp"
#include <fmt/core.h>
#include <type_traits>

template<typename T>
static void log(const T& t)
{
    static file::file_raii logger("./logs", "w");

    if constexpr (std::is_same_v<T, const char*>)
        logger.writeline(t);
    else if constexpr (std::is_same_v<T, str>)
        logger.writeline(t.chars());
    else if constexpr (std::is_same_v<T, std::string>)
        logger.writeline(t.c_str());
    else
        logger.writeline(fmt::format("{}", t).c_str());
}
