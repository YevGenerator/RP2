#include "../include/rpdefParser/RpdefSymbolHasher.hpp"
#include <gtest/gtest.h>

#include "ConfigParser.hpp"
#include "HashMap.hpp"
#include "SystemConfig.hpp"
#include "gmock/gmock.h"

TEST(ParserTest, HasherCorrectSymbols) {
    constexpr RpdefConfig::RpdefSymbolHasher hasher;
    EXPECT_EQ(hasher('i'), 0);
    EXPECT_EQ(hasher('b'), 1);
    EXPECT_EQ(hasher('w'), 2);
    EXPECT_EQ(hasher('l'), 3);
    EXPECT_EQ(hasher('n'), 4);
}

TEST(ParserTest, HashMapInitAfterCreation) {
    RpdefConfig::HashMap<int> map;
    map['i'] = 2;
    map['b'] = 3;
    map['n'] = 4;
    EXPECT_EQ(map['i']+map['b']+map['n'], 2+3+4);

    RpdefConfig::HashMap<int(*)()> map2;
    map2['i'] = []() { return 0; };
    map2['b'] = []() { return 3; };
    map2['n'] = []() { return 10; };
    EXPECT_EQ(map2['i']()+map2['b']()+map2['n'](), 0+3+10);
}

TEST(ParserTest, ConfigParsingFromTextSimpleValues) {
    RpdefConfig::ConfigParser parser;
    const auto str = "i32w55";
    auto config = parser.parseFromString(str);
    EXPECT_EQ(config.id, 32);
    EXPECT_EQ(config.workerCount, 55);

    const auto str2 = "i2344w5l2l10l300l50-60";
    config = parser.parseFromString(str2);
    EXPECT_EQ(config.id, 2344);
    EXPECT_EQ(config.workerCount, 5);
    EXPECT_EQ(config.levelsToCreate.size(), 3+11);
    EXPECT_THAT(config.levelsToCreate, testing::ElementsAre(2, 10 , 300, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60));
}

TEST(ParserTest, ConfigParsingNodeBindsSimpleValues) {
    RpdefConfig::ConfigParser parser;
    const auto str = "i2344w5l2l10n1.3n4.3n5.5n100.444b1.1>2.3b44.55>55.44";
    auto config = parser.parseFromString(str);
    EXPECT_EQ(config.id, 2344);
    EXPECT_EQ(config.workerCount, 5);
    EXPECT_THAT(config.levelsToCreate, testing::ElementsAre(2, 10));
    EXPECT_EQ(config.nodesToCreate.size(), 4);
    EXPECT_THAT(config.nodesToCreate, testing::ElementsAre(
                    NodeSystem::NodeId{1, 3},
                    NodeSystem::NodeId{4,3},
                    NodeSystem::NodeId{5,5},
                    NodeSystem::NodeId{100, 444}));
    EXPECT_EQ(config.bindingsToCreate.size(), 2);
    using NodePair = std::pair<NodeSystem::NodeId, NodeSystem::NodeId>;
    EXPECT_THAT(config.bindingsToCreate, testing::ElementsAre(
                    NodePair{{1, 1}, {2, 3}},
                    NodePair{{44, 55},{55, 44}}));
}

TEST(ParserTest, ConfigParsingNodeBindsComplexValues) {
    RpdefConfig::ConfigParser parser;
    const auto str = "i2344w5l2l10n1.3-6n5-7.5n2-4.1-3b1-3.1-2>2-4.3";
    auto config = parser.parseFromString(str);
    using N = NodeSystem::NodeId;
    using NP = std::pair<N, N>;
    EXPECT_EQ(config.id, 2344);
    EXPECT_EQ(config.workerCount, 5);
    EXPECT_THAT(config.levelsToCreate, testing::ElementsAre(2, 10));
    EXPECT_EQ(config.nodesToCreate.size(), 4 + 3 + 3 * 3);
    EXPECT_THAT(config.nodesToCreate, testing::ElementsAre(
                    N{1, 3}, N{1, 4}, N{1, 5}, N{1, 6},
                    N{5, 5}, N{6, 5}, N{7, 5},
                    N{2, 1}, N{2, 2}, N{2, 3},
                    N{3, 1}, N{3, 2}, N{3, 3},
                    N{4, 1}, N{4, 2}, N{4, 3}
                ));
    EXPECT_EQ(config.bindingsToCreate.size(), 18);
    EXPECT_THAT(config.bindingsToCreate, testing::ElementsAre(
                    NP{{1, 1}, {2, 3}},
                    NP{{1, 1}, {3, 3}},
                    NP{{1, 1}, {4, 3}},
                    NP{{1, 2}, {2, 3}},
                    NP{{1, 2}, {3, 3}},
                    NP{{1, 2}, {4, 3}},
                    NP{{2, 1}, {2, 3}},
                    NP{{2, 1}, {3, 3}},
                    NP{{2, 1}, {4, 3}},
                    NP{{2, 2}, {2, 3}},
                    NP{{2, 2}, {3, 3}},
                    NP{{2, 2}, {4, 3}},
                    NP{{3, 1}, {2, 3}},
                    NP{{3, 1}, {3, 3}},
                    NP{{3, 1}, {4, 3}},
                    NP{{3, 2}, {2, 3}},
                    NP{{3, 2}, {3, 3}},
                    NP{{3, 2}, {4, 3}}
                ));
}
