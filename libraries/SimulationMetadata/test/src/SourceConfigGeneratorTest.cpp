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

    EXPECT_EQ(fSourceConfig.fGeneratorType, "point");
}

TEST(SourceConfigGenerator, DeserializePlane) {
    const auto file = fs::path(GENERATORS_PATH + "plane.yaml");

    YAML::Node config = YAML::LoadFile(file);

    cout << "FILE " << file << " CONTENTS: " << endl;
    cout << config << endl;

    EXPECT_TRUE(config["source"]);

    SourceConfig fSourceConfig;

    fSourceConfig.Deserialize(config["source"]);

    fSourceConfig.Print();

    EXPECT_EQ(fSourceConfig.fGeneratorType, "plane");
    EXPECT_EQ(fSourceConfig.fGeneratorPosition, TVector3({0, 200, 0}));
    EXPECT_EQ(fSourceConfig.fGeneratorDirection, TVector3({0, 1, 1}));
    EXPECT_EQ(fSourceConfig.fGeneratorSize, TVector3({10, 0, 20}));
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

    EXPECT_EQ(fSourceConfig.fGeneratorType, "disk");
    EXPECT_EQ(fSourceConfig.fGeneratorPosition, TVector3({10, 0, 0}));
    EXPECT_EQ(fSourceConfig.fGeneratorDirection, TVector3({1, 0, 0}));
    EXPECT_EQ(fSourceConfig.fGeneratorDiameter, 15);
}