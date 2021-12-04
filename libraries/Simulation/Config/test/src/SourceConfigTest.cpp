//
// Created by lobis on 30/11/2021.
//

#include <gtest/gtest.h>

#include <filesystem>

#include "SourceConfig.h"

using namespace std;

namespace fs = std::filesystem;

#define FILES_PATH string(std::filesystem::path(__FILE__).parent_path().parent_path()) + "/files/"

TEST(SourceConfig, DeserializeParticle) {
    const auto file = fs::path(FILES_PATH + "source.yaml");

    YAML::Node config = YAML::LoadFile(file);

    EXPECT_TRUE(config["source"]);

    SourceConfig fSourceConfig;

    fSourceConfig.Deserialize(config["source"]);

    fSourceConfig.Print();

    EXPECT_EQ(fSourceConfig.fParticleName, "gamma");
}