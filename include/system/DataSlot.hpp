#pragma once
#include <chrono>

namespace NodeSystem {
    struct DataSlot {
        double data{0.0};
        std::chrono::steady_clock::time_point time;
    };
}
