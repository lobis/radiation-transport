//
// Created by lobis on 30/11/2021.
//

#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "SourceConfig.h"

using namespace std;

namespace fs = std::filesystem;

#define ANGULAR_PATH string(std::filesystem::path(__FILE__).parent_path().parent_path()) + "/files/angular/"

TEST(SourceConfigAngular, DeserializeFlux) {
    const auto file = fs::path(ANGULAR_PATH + "flux.yaml");

    YAML::Node config = YAML::LoadFile(file);

    cout << "FILE " << file << " CONTENTS: " << endl;
    cout << config << endl;

    EXPECT_TRUE(config["source"]);

    SourceConfig fSourceConfig;

    fSourceConfig.Deserialize(config["source"]);

    fSourceConfig.Print();

    EXPECT_EQ(fSourceConfig.fAngularDistributionType, "flux");
    EXPECT_EQ(fSourceConfig.fAngularDistributionDirection, TVector3({-1, 1, 2}));
}
