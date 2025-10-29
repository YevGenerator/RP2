#include "../include/Printer.hpp"

#include <iostream>

std::mutex Printer::cout_mutex;

void Printer::print_safe(const std::string &msg) {
    std::lock_guard lock(cout_mutex);
    std::cout << msg << std::endl;
}
