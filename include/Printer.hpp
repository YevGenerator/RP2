#pragma once

#include <mutex>
#include <string>

#include "system/SystemTemplate.hpp"

class Printer {
public:
    static std::mutex cout_mutex;

    static void print_safe(const std::string &msg);
    static void print_startedProxy(const NodeSystem::SystemTemplate& system) {
        print_safe("[Proxy] Проксі запущено. Слухаю " + system.queueTaskIn() +
                                   ", відправляю на " + system.queueTaskOut());
    }

};
