#pragma once

#include <mutex>
#include <string>

class Printer {
public:
    static std::mutex cout_mutex;

    static void print_safe(const std::string &msg);
};
