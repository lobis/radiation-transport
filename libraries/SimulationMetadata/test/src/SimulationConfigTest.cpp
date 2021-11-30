//
// Created by lobis on 11/22/2021.
//

#include <gtest/gtest.h>

#include <filesystem>

#include "SimulationConfig.h"

using namespace std;

#define BASE_PATH string(std::filesystem::path(__FILE__).parent_path().parent_path()) + "/"

TEST(SimulationConfig, LoadFromFile) {
    const auto config = SimulationConfig(BASE_PATH + "files/simulation.yaml");

    EXPECT_EQ(config.fRunManagerType, "serial");
}

TEST(SimulationConfig, DefaultConfig) {
    const SimulationConfig config;

    EXPECT_EQ(config.fVerboseLevel, "info");
    EXPECT_EQ(config.fRunManagerType, "serial");
    EXPECT_EQ(config.fThreads, 2);
    EXPECT_EQ(config.fSeed, 0);
    EXPECT_EQ(config.fCommands.empty(), true);
}