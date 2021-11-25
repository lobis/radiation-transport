//
// Created by lobis on 11/22/2021.
//

#include <gtest/gtest.h>

#include <filesystem>

#include "DetectorConfig.h"

using namespace std;

namespace fs = std::filesystem;

auto testsBasePath1 = string(std::filesystem::path(__FILE__).parent_path().parent_path()) + "/";

TEST(DetectorConfig, Deserialize) {
    const auto file = fs::path(testsBasePath1 + "files/detector.yaml");

    YAML::Node config = YAML::LoadFile(file);

    EXPECT_TRUE(config["detector"]);

    DetectorConfig detectorConfig;

    detectorConfig.Deserialize(config["detector"]);

    detectorConfig.Print();

    EXPECT_EQ(detectorConfig.fGeometryFilename, "test.gdml");
}
