//
// Created by lobis on 30/11/2021.
//

#include <gtest/gtest.h>

#include <filesystem>

#include "SourceConfig.h"

using namespace std;

namespace fs = std::filesystem;

#define SOURCES_PATH string(std::filesystem::path(__FILE__).parent_path().parent_path()) + "/files/sources/"

TEST(SourceConfig, DeserializePoint) {
    const auto file = fs::path(SOURCES_PATH + "point.yaml");

    YAML::Node config = YAML::LoadFile(file);

    EXPECT_TRUE(config["source"]);

    SourceConfig fSourceConfig;

    fSourceConfig.Deserialize(config["source"]);

    fSourceConfig.Print();

    EXPECT_EQ(fSourceConfig.fGeneratorType, "point");
}

TEST(SourceConfig, DeserializePlane) {
    const auto file = fs::path(SOURCES_PATH + "plane.yaml");

    YAML::Node config = YAML::LoadFile(file);

    EXPECT_TRUE(config["source"]);

    SourceConfig fSourceConfig;

    fSourceConfig.Deserialize(config["source"]);

    fSourceConfig.Print();

    EXPECT_EQ(fSourceConfig.fGeneratorType, "plane");
}

TEST(SourceConfig, DeserializeDisk) {
    const auto file = fs::path(SOURCES_PATH + "disk.yaml");

    YAML::Node config = YAML::LoadFile(file);

    EXPECT_TRUE(config["source"]);

    SourceConfig fSourceConfig;

    fSourceConfig.Deserialize(config["source"]);

    fSourceConfig.Print();

    EXPECT_EQ(fSourceConfig.fGeneratorType, "disk");
}