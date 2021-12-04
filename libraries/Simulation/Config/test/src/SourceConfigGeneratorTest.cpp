//
// Created by lobis on 30/11/2021.
//

#include <gtest/gtest.h>

#include <filesystem>

#include "SourceConfig.h"

using namespace std;

namespace fs = std::filesystem;

#define GENERATORS_PATH string(std::filesystem::path(__FILE__).parent_path().parent_path()) + "/files/generators/"

TEST(SourceConfigGenerator, DeserializePoint) {
    const auto file = fs::path(GENERATORS_PATH + "point.yaml");

    YAML::Node config = YAML::LoadFile(file);

    cout << "FILE " << file << " CONTENTS: " << endl;
    cout << config << endl;

    EXPECT_TRUE(config["source"]);

    SourceConfig fSourceConfig;

    fSourceConfig.Deserialize(config["source"]);

    fSourceConfig.Print();

    EXPECT_EQ(fSourceConfig.fPositionDistributionType, "point");
}

TEST(SourceConfigGenerator, DeserializeRectangle) {
    const auto file = fs::path(GENERATORS_PATH + "rectangle.yaml");

    YAML::Node config = YAML::LoadFile(file);

    cout << "FILE " << file << " CONTENTS: " << endl;
    cout << config << endl;

    EXPECT_TRUE(config["source"]);

    SourceConfig fSourceConfig;

    fSourceConfig.Deserialize(config["source"]);

    fSourceConfig.Print();

    EXPECT_EQ(fSourceConfig.fPositionDistributionType, "rectangle");
    EXPECT_EQ(fSourceConfig.fPositionDistributionCenter, TVector3({0, 200, 0}));
    EXPECT_EQ(fSourceConfig.fPositionDistributionOrientation, TVector3({0, 1, 1}));
    EXPECT_EQ(fSourceConfig.fPositionDistributionRectangleDimensions.x(), 55);
    EXPECT_EQ(fSourceConfig.fPositionDistributionRectangleDimensions.y(), 23);
    EXPECT_EQ(fSourceConfig.fPositionDistributionRectangleDimensions.z(), 0);
}

TEST(SourceConfigGenerator, DeserializeSquare) {
    const auto file = fs::path(GENERATORS_PATH + "square.yaml");

    YAML::Node config = YAML::LoadFile(file);

    cout << "FILE " << file << " CONTENTS: " << endl;
    cout << config << endl;

    EXPECT_TRUE(config["source"]);

    SourceConfig fSourceConfig;

    fSourceConfig.Deserialize(config["source"]);

    fSourceConfig.Print();

    EXPECT_EQ(fSourceConfig.fPositionDistributionType, "square");
    EXPECT_EQ(fSourceConfig.fPositionDistributionCenter, TVector3({0, 200, 0}));
    EXPECT_EQ(fSourceConfig.fPositionDistributionOrientation, TVector3({0, 1, 1}));
    EXPECT_EQ(fSourceConfig.fPositionDistributionSquareSide, 25);
}

TEST(SourceConfigGenerator, DeserializeDisk) {
    const auto file = fs::path(GENERATORS_PATH + "disk.yaml");

    YAML::Node config = YAML::LoadFile(file);

    cout << "FILE " << file << " CONTENTS: " << endl;
    cout << config << endl;

    EXPECT_TRUE(config["source"]);

    SourceConfig fSourceConfig;

    fSourceConfig.Deserialize(config["source"]);

    fSourceConfig.Print();

    EXPECT_EQ(fSourceConfig.fPositionDistributionType, "disk");
    EXPECT_EQ(fSourceConfig.fPositionDistributionCenter, TVector3({10, 0, 0}));
    EXPECT_EQ(fSourceConfig.fPositionDistributionOrientation, TVector3({1, 0, 0}));
    EXPECT_EQ(fSourceConfig.fPositionDistributionDiameter, 15);
}