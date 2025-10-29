cmake_minimum_required(VERSION 4.0)
project(RP2)

set(CMAKE_CXX_STANDARD 20)

add_executable(RP2 main.cpp
        include/NodeId.hpp
        include/Node.hpp
        include/Printer.hpp
        include/Message.hpp
        include/DataSlot.hpp
        include/SensorNode.hpp
        include/Worker.hpp
        include/NodeStore.hpp
        include/Router.hpp
        include/NetworkManager.hpp
        include/NetworkConfig.hpp
        src/Printer.cpp
        src/Node.cpp
        src/NodeId.cpp
        src/SensorNode.cpp
        src/NodeStore.cpp
        src/Worker.cpp
        src/Router.cpp
        src/NetworkManager.cpp)
find_package(ZeroMQ CONFIG REQUIRED)
target_link_libraries(RP2 PRIVATE libzmq)