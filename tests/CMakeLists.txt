cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 20)

project(RP2Test)

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.17.0
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests
        ../src/system/NodeId.cpp
        ../src/parser/ConfigParser.cpp
        test_parser.cpp
        ../include/rpdefParser/Deparser.hpp
        test_deparser.cpp
        test_parser_deparser.cpp
)

target_include_directories(tests PRIVATE
        ../include
        ../include/rpdefParser
        ../include/system
)
target_link_libraries(tests gtest_main gmock)

include(GoogleTest)
gtest_discover_tests(tests)
