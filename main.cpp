#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <fstream>

#include "include/system/SystemTemplate.hpp"
#include "include/Printer.hpp"
#include "windows.h"
#include "include/rpdefParser/ConfigParser.hpp"
#include "include/rpdefParser/Deparser.hpp"
#include "include/system/SensorManager.hpp"
/*
void processCommand(const std::string &line, NodeSystem::SystemTemplate &manager) {
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
        } else if (cmd == "exit") {
            exit(0);
        } else if (cmd == "help") {
            Printer::print_safe(
                "Команди: help | level <L> | sensor <L> <I> | node <L> <I> | bind <L1> <I1> <L2> <I2> | trigger <L> <I> | sleep <ms> | exit");
        } else {
            Printer::print_safe("Невідома команда. Введіть 'help'.");
        }
    } catch (const std::exception &e) {
        Printer::print_safe(std::string("Помилка виконання команди: ") + e.what());
    }
}
*/
void fromConfig(const RpdefConfig::SystemConfig &config, NodeSystem::SystemTemplate &system) {
    system.setId(config.id);
    system.setWorkers(config.workerCount);
    for (const auto lvl : config.levelsToCreate) {
        system.createLevel(lvl);
    }
    for (const auto node: config.nodesToCreate) {
        system.createNode(node);
    }
    for (const auto bind: config.bindingsToCreate) {
        system.bind(bind.first, bind.second);
    }
}
int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // *** 1. СТВОРИТИ СПІЛЬНИЙ КОНТЕКСТ ***
    zmq::context_t main_context(1);
    NodeSystem::SystemTemplate manager1(1, &main_context); // ID 1
    NodeSystem::SystemTemplate manager2(2, &main_context); // ID 1
    RpdefConfig::ConfigParser parser, parser2;
    auto config1 = parser.parse("manager1.rpdef");
    auto config2 = parser2.parse("manager2.rpdef");
    fromConfig(config1, manager1);
    fromConfig(config2, manager2);
    manager1.bindToSystem(manager2, 1);

    manager1.run_async();
    manager2.run_async();

    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Час на запуск

    // *** 3. Створити Менеджер Сенсорів ***
    NodeSystem::SensorManager sensorManager(&main_context);

    // Сенсор 102 -> manager1, вузол {0, 1}
    sensorManager.addSensor(102, manager1.queueTaskIn(), {0, 1});

    // Сенсор 103 -> manager2, вузол {0, 5}
    sensorManager.addSensor(103, manager2.queueTaskIn(), {0, 2});

    // Запускаємо єдиний потік сенсорів
    sensorManager.start();

    // *** 4. Прив'язка Систем ***
    // (Припускаємо, що ви оновили `bindToSystem` для коректної роботи з контекстом)
    while (true) {

    }
    /*
    RpdefConfig::SystemConfig config;
    config.id = 2;
    config.workerCount = 2;
    config.levelsToCreate = {0, 1};
    config.nodesToCreate = {
        {0, 1}, {0, 2},
        {1, 1}, {1, 2}, {1, 3}, {1, 4}
    };
    config.bindingsToCreate = {
        {{0, 1}, {1, 1}},
        {{0, 1}, {1, 2}},
        {{0, 1}, {1, 3}},
        {{0, 2}, {1, 2}},
        {{0, 2}, {1, 3}},
        {{0, 2}, {1, 4}},
    };
    NodeSystem::SystemTemplate manager1(1, &main_context); // ID 1
    fromConfig(config, manager1);
    auto str = RpdefConfig::Deparser::deparseConfig(config);
    {
        std::ofstream file("manager2.rpdef");
        file << str;
    }
    */
    /*
    // *** 2. Створити системи, передавши контекст ***
    // (Припускаємо, що ви оновили конструктор SystemTemplate)
    NodeSystem::SystemTemplate manager1(1, &main_context); // ID 1
    NodeSystem::SystemTemplate manager2(2, &main_context); // ID 2
    manager1.bindToSystem(manager2, 1);
    // (Тут логіка завантаження .rpdef та applyConfig)
    // ...
    // manager1.applyConfig(...);
    // manager2.applyConfig(...);

    manager1.run_async(2);
    manager2.run_async(2);

    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Час на запуск

    // *** 3. Створити Менеджер Сенсорів ***
    SensorManager sensorManager(&main_context);

    // "Прив'язка" сенсорів
    // Сенсор 101 -> manager1, вузол {0, 0}
    sensorManager.addSensor(101, manager1.queueTaskIn(), {0, 0});

    // Сенсор 102 -> manager1, вузол {0, 1}
    sensorManager.addSensor(102, manager1.queueTaskIn(), {0, 1});

    // Сенсор 103 -> manager2, вузол {0, 5}
    sensorManager.addSensor(103, manager2.queueTaskIn(), {0, 5});

    // Запускаємо єдиний потік сенсорів
    sensorManager.start();

    // *** 4. Прив'язка Систем ***
    // (Припускаємо, що ви оновили `bindToSystem` для коректної роботи з контекстом)
    manager1.bindToSystem(manager2, 0);
    SystemTemplate manager;
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
    */
}
