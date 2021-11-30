//
// Created by lobis on 30/11/2021.
//

#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "SourceConfig.h"

using namespace std;

namespace fs = std::filesystem;

#define ENERGIES_PATH string(std::filesystem::path(__FILE__).parent_path().parent_path()) + "/files/energies/"

TEST(SourceConfigEnergy, DeserializeMono) {
    const auto file = fs::path(ENERGIES_PATH + "mono.yaml");

    YAML::Node config = YAML::LoadFile(file);

    cout << "FILE " << file << " CONTENTS: " << endl;
    cout << config << endl;

    EXPECT_TRUE(config["source"]);

    SourceConfig fSourceConfig;

    fSourceConfig.Deserialize(config["source"]);

    fSourceConfig.Print();

    EXPECT_EQ(fSourceConfig.fEnergyDistributionType, "mono");
    EXPECT_EQ(fSourceConfig.fEnergyDistributionUnit, "MeV");
    EXPECT_EQ(fSourceConfig.fEnergyDistributionMonoValue, 120.5);
}