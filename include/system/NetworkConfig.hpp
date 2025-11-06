#pragma once
#include <string_view>

namespace NetworkConfig {
    inline constexpr std::string_view TaskQueueIn = "inproc://tasks_in";
    inline constexpr std::string_view TaskQueueOut = "inproc://tasks_out";
    inline constexpr std::string_view OutputQueue = "inproc://outputs";
}
