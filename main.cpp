#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>

#include "include/NetworkManager.hpp"
#include "include/Printer.hpp"
#include "windows.h"

void processCommand(const std::string& line, NetworkManager& manager) {
    std::stringstream ss(line);
    std::string cmd;
    ss >> cmd;

    try {
        if (cmd == "level") {
            int l;
            ss >> l;
            manager.createLevel(l);
        } else if (cmd == "sensor") {
            int l, i;
            ss >> l >> i;
            manager.createSensorNode({l, i});
        } else if (cmd == "node") {
            int l, i;
            ss >> l >> i;
            manager.createNode({l, i});
        } else if (cmd == "bind") {
            int l1, i1, l2, i2;
            ss >> l1 >> i1 >> l2 >> i2;
            manager.bind({l1, i1}, {l2, i2});
        } else if (cmd == "trigger") {
            int l, i;
            ss >> l >> i;
            manager.triggerSensor({l, i});
        } else if (cmd == "sleep") {
            int ms;
            ss >> ms;
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        }
        else if (cmd == "exit") {
            exit(0);
        } else if (cmd == "help") {
             Printer::print_safe("Команди: help | level <L> | sensor <L> <I> | node <L> <I> | bind <L1> <I1> <L2> <I2> | trigger <L> <I> | sleep <ms> | exit");
        }
        else {
            Printer::print_safe("Невідома команда. Введіть 'help'.");
        }
    } catch (const std::exception& e) {
        Printer::print_safe(std::string("Помилка виконання команди: ") + e.what());
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    NetworkManager manager;
    manager.run_async(2);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    Printer::print_safe("Система запущена. Введіть 'help' для списку команд.");

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line) || line == "exit") {
            break;
        }
        if (line.empty()) continue;
        processCommand(line, manager);
    }

    return 0;
}