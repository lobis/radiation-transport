//
// Created by lobis on 11/22/2021.
//

#include <gtest/gtest.h>

#include "SimulationConfig.h"

TEST(SimulationConfig, LoadFromFile) {
    const auto config = SimulationConfig::LoadFromFile("files/simulation.yaml");

    EXPECT_EQ(config.fSerialMode, "serial");
}
