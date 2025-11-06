#include <fstream>

#include "../include/rpdefParser/Deparser.hpp"
#include "../include/rpdefParser/ConfigParser.hpp"
#include <gtest/gtest.h>
#include "gmock/gmock.h"

using Deparse = RpdefConfig::Deparser;
using Parse = RpdefConfig::ConfigParser;
TEST(ParserDeparserTest, IdWorkerParser) {
    RpdefConfig::SystemConfig config;
    Parse parser;
    config.id = 200;
    config.workerCount = 300;
    auto deparsed = Deparse::deparseConfig(config);
    auto parsed = parser.parseFromString(deparsed);
    EXPECT_EQ(config.id, parsed.id);
    EXPECT_EQ(config.workerCount, parsed.workerCount);
}

TEST(ParserDeparserTest, IdWorkerFileWriteParser) {
    RpdefConfig::SystemConfig config;
    Parse parser;
    config.id = 200;
    config.workerCount = 300;
    auto deparsed = Deparse::deparseConfig(config);
    {
        std::ofstream file("temp.txt");
        file << deparsed;
    }
    std::string content;
    {
        std::ifstream file("temp.txt");
        content = std::string((std::istreambuf_iterator(file)),
                              std::istreambuf_iterator<char>());
    }
    auto parsed = parser.parseFromString(content);
    EXPECT_EQ(config.id, parsed.id);
    EXPECT_EQ(config.workerCount, parsed.workerCount);
}

TEST(ParserDeparserTest, NodesFullParser) {
    RpdefConfig::SystemConfig config;
    Parse parser;
    config.id = 200;
    config.workerCount = 300;
    config.levelsToCreate = {0, 1, 2, 4, 5, 6, 10, 12};
    config.nodesToCreate = {
        {0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2},
        {10, 11}, {22, 33}, {100, 105}, {100, 106},
        {205, 200}, {206, 200}, {207, 200}
    };
    config.bindingsToCreate = {
        {{0, 0}, {1, 0}}, {{0, 1}, {1, 0}}, {{100, 200}, {300, 500}}
    };
    auto deparsed = Deparse::deparseConfig(config);
    {
        std::ofstream file("temp.txt");
        file << deparsed;
    }
    std::string content;
    {
        std::ifstream file("temp.txt");
        content = std::string((std::istreambuf_iterator(file)),
                              std::istreambuf_iterator<char>());
    }
    auto parsed = parser.parseFromString(content);
    EXPECT_EQ(config.id, parsed.id);
    EXPECT_EQ(config.workerCount, parsed.workerCount);
    EXPECT_THAT(config.levelsToCreate, testing::UnorderedElementsAreArray(parsed.levelsToCreate));
    EXPECT_THAT(config.nodesToCreate, testing::UnorderedElementsAreArray(parsed.nodesToCreate));
    EXPECT_THAT(config.bindingsToCreate, testing::UnorderedElementsAreArray(parsed.bindingsToCreate));
}
