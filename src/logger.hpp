#pragma once

#include <fmt/core.h>
#include <type_traits>

#include "file_io.hpp"

template<typename T>
static void log(const T& t)
{
    static file::file_raii logger("./logs", "w");

    if constexpr (std::is_same_v<T, const char*>)
        logger.write_line(t);
    else if constexpr (std::is_same_v<T, str>)
        logger.write_line(t.chars());
    else if constexpr (std::is_same_v<T, std::string>)
        logger.write_line(t.c_str());
    else
        logger.write_line(fmt::format("{}", t).c_str());
}
