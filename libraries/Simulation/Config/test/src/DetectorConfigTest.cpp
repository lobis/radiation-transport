//
// Created by lobis on 11/22/2021.
//

#include <gtest/gtest.h>

#include <filesystem>

#include "DetectorConstructionConfig.h"

#define BASE_PATH string(std::filesystem::path(__FILE__).parent_path().parent_path()) + "/"

using namespace std;

namespace fs = std::filesystem;

TEST(DetectorConfig, Deserialize) {
    const auto file = fs::path(BASE_PATH + "files/detector.yaml");

    YAML::Node config = YAML::LoadFile(file);

    EXPECT_TRUE(config["detector"]);

    DetectorConstructionConfig detectorConfig;

    detectorConfig.Deserialize(config["detector"]);

    detectorConfig.Print();

    EXPECT_EQ(detectorConfig.fGeometryFilename, "test.gdml");
}

TEST(DetectorConfig, SensitiveAndKill) {
    const auto file = fs::path(BASE_PATH + "files/detector.yaml");

    YAML::Node config = YAML::LoadFile(file);

    EXPECT_TRUE(config["detector"]);

    DetectorConstructionConfig detectorConfig;

    detectorConfig.Deserialize(config["detector"]);

    detectorConfig.Print();

    for (const auto& volume : detectorConfig.fVolumes) {
        if (volume.fName == "sensitiveVolume") {
            EXPECT_TRUE(volume.fIsSensitive == true);
            EXPECT_TRUE(volume.fKill == false);
        } else if (volume.fName == "notSensitiveVolume") {
            EXPECT_TRUE(volume.fIsSensitive == false);
            EXPECT_TRUE(volume.fKill == false);
        } else if (volume.fName == "sensitiveVolumeWithKill") {
            EXPECT_TRUE(volume.fIsSensitive == true);
            EXPECT_TRUE(volume.fKill == true);
        } else {
            throw std::runtime_error("invalid volume name, please update configs");
        }
    }

    EXPECT_EQ(detectorConfig.fGeometryFilename, "test.gdml");
}