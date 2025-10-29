#pragma once
#include <string_view>

namespace NetworkConfig {
    inline constexpr std::string_view TaskQueue = "inproc://tasks";
    inline constexpr std::string_view OutputQueue = "inproc://outputs";
}
