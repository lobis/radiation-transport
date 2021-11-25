//
// Created by lobis on 11/22/2021.
//

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <filesystem>

#include "Application.h"
#include "Exceptions.h"

using namespace std;

auto examplesPath = string(std::filesystem::path(__FILE__).parent_path().parent_path().parent_path()) + "/examples/";

TEST(ApplicationTest, ShowUsage) {
    Application app;
    app.ShowUsage();
}

TEST(ApplicationTest, LoadConfigFromFile) {
    const string configFile = examplesPath + "basic/simulation.yaml";

    Application app;

    app.LoadConfigFromFile(configFile);

    app.PrintConfig();
}

TEST(ApplicationTest, NoGeometryFile) {
    SimulationConfig config(examplesPath + "basic/simulation.yaml");

    config.fDetectorConfig.fGeometryFilename = "";

    Application app;

    app.LoadConfigFromFile(config);

    app.PrintConfig();

    try {
        app.UserInitialization();
    } catch (exception& ex) {
        const auto targetException = (exception*)&exceptions::NoGeometryFile;
        EXPECT_STREQ(ex.what(), targetException->what());
    }
}

TEST(ApplicationTest, GeometryFileNotFound) {
    SimulationConfig config(examplesPath + "basic/simulation.yaml");

    config.fDetectorConfig.fGeometryFilename = "/tmp/geometry-does-not-exist.gdml";

    Application app;

    app.LoadConfigFromFile(config);

    app.PrintConfig();

    try {
        app.UserInitialization();
    } catch (exception& ex) {
        const auto targetException = (exception*)&exceptions::GeometryFileNotFound;
        EXPECT_STREQ(ex.what(), targetException->what());
    }
}