cmake_minimum_required(VERSION 4.0)
project(RP2)

set(CMAKE_CXX_STANDARD 20)

add_executable(RP2 main.cpp
        include/System/NodeId.hpp
        include/System/Node.hpp
        include/Printer.hpp
        include/System/Message.hpp
        include/System/DataSlot.hpp
        include/System/Worker.hpp
        include/System/NodeStore.hpp
        include/System/Router.hpp
        include/System/SystemTemplate.hpp
        include/System/NetworkConfig.hpp
        src/Printer.cpp
        src/system/Node.cpp
        src/system/NodeId.cpp
        src/system/NodeStore.cpp
        src/system/Worker.cpp
        src/system/Router.cpp
        src/system/SystemTemplate.cpp
        src/parser/ConfigParser.cpp
        include/rpdefParser/ConfigParser.hpp
        include/rpdefParser/SystemConfig.hpp
        include/rpdefParser/RpdefSymbolHasher.hpp
        include/rpdefParser/HashMap.hpp
        src/system/SensorManager.cpp
        include/system/SensorManager.hpp
        include/system/SensorManager.hpp
)
find_package(ZeroMQ CONFIG REQUIRED)
target_link_libraries(RP2 PRIVATE libzmq)