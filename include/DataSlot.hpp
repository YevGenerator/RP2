#pragma once
#include <chrono>


struct DataSlot {
    double data{0.0};
    std::chrono::steady_clock::time_point time;
};
