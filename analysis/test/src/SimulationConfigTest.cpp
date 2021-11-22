//
// Created by lobis on 11/22/2021.
//

#include <gtest/gtest.h>

#include <filesystem>

#include "SimulationConfig.h"

using namespace std;

auto testsBasePath = string(std::filesystem::path(__FILE__).parent_path().parent_path()) + "/";

TEST(SimulationConfig, LoadFromFile) {
    const auto config = SimulationConfig::LoadFromFile(testsBasePath + "files/simulation.yaml");

    EXPECT_EQ(config.fSerialMode, "serial");
}
