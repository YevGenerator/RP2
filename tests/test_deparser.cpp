#include "../include/rpdefParser/Deparser.hpp"
#include <gtest/gtest.h>

#include "gmock/gmock.h"
using Parse = RpdefConfig::Deparser;
TEST(DeparserTest, SingleSequenceLevels) {
    RpdefConfig::SystemConfig config;
    config.levelsToCreate = {0, 1, 2, 3, 4, 5};
    auto str = Parse::deparseLevels(config.levelsToCreate);
    EXPECT_EQ(str, "l0-5");
}

TEST(DeparserTest, SeveralSequencesLevels) {
    RpdefConfig::SystemConfig config;
    config.levelsToCreate = {0, 2, 3, 4, 7, 10, 11, 12, 15};
    auto str = Parse::deparseLevels(config.levelsToCreate);
    EXPECT_EQ(str, "l0l2-4l7l10-12l15");
    config.levelsToCreate = {0, 1, 3, 4, 5, 10, 11, 12, 15, 16};
    auto str2 = Parse::deparseLevels(config.levelsToCreate);
    EXPECT_EQ(str2, "l0-1l3-5l10-12l15-16");
}

TEST(DeparserTest, NoRangeLevels) {
    RpdefConfig::SystemConfig config;
    config.levelsToCreate = {0};
    auto str = Parse::deparseLevels(config.levelsToCreate);
    EXPECT_EQ(str, "l0");
    config.levelsToCreate = {0, 2, 7};
    auto str2 = Parse::deparseLevels(config.levelsToCreate);
    EXPECT_EQ(str2, "l0l2l7");
}

TEST(DeparserTest, DifferentSequenceNodes) {
    RpdefConfig::SystemConfig config;
    config.nodesToCreate = {{0, 0}, {1, 1}, {2, 2}};
    auto str = Parse::deparseNodes(config.nodesToCreate);
    EXPECT_EQ(str, "n0.0n1.1n2.2");
}

TEST(DeparserTest, SingeSequenceNodes) {
    RpdefConfig::SystemConfig config;
    config.nodesToCreate = {
        {0, 0}, {0, 1}, {0, 2},
        {1, 4}, {1, 5}, {1, 6}
    };
    auto str = Parse::deparseNodes(config.nodesToCreate);
    EXPECT_EQ(str, "n0.0-2n1.4-6");
    config.nodesToCreate = {
        {0, 0}, {1, 0}, {2, 0}, {4, 0},
        {10, 4}, {11, 4}, {12, 4}, {15, 4}
    };
    auto str2 = Parse::deparseNodes(config.nodesToCreate);
    EXPECT_EQ(str2, "n0-2.0n4.0n10-12.4n15.4");
}

TEST(DeparserTest, MatrixSequenceNodes) {
    RpdefConfig::SystemConfig config;
    config.nodesToCreate = {
        {0, 0}, {0, 1}, {0, 2},
        {1, 0}, {1, 1}, {1, 2},
        {2, 0}, {2, 1}, {2, 2},
    };
    auto str = Parse::compressNodes(config.nodesToCreate);
    EXPECT_EQ(str, "n0-2.0-2");
    config.nodesToCreate = {
        {0, 0}, {0, 1}, {0, 2},
        {1, 0}, {1, 1}, {1, 2},
        {3, 5}, {3, 6}, {4, 6}, {5, 6},
        {10, 10}, {10, 11}, {11, 10},
    };
    auto str2 = Parse::compressNodes(config.nodesToCreate);
    EXPECT_EQ(str2, "n0-1.0-2n11.10n10.10-11n3.5-6n4-5.6");
}
