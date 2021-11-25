//
// Created by lobis on 11/22/2021.
//

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <filesystem>

#include "Application.h"
#include "Exceptions.h"

#define EXAMPLES_PATH string(std::filesystem::path(__FILE__).parent_path().parent_path().parent_path()) + "/examples/"

using namespace std;

TEST(Application, ShowUsage) {
    Application app;
    app.ShowUsage();
}

TEST(Application, LoadConfigFromFile) {
    const string configFile = EXAMPLES_PATH + "basic/simulation.yaml";

    Application app;

    app.LoadConfigFromFile(configFile);

    app.PrintConfig();
}

TEST(Application, NoGeometryFile) {
    SimulationConfig config(EXAMPLES_PATH + "basic/simulation.yaml");

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

TEST(Application, GeometryFileNotFound) {
    SimulationConfig config(EXAMPLES_PATH + "basic/simulation.yaml");

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

TEST(Application, LoadTextGeometry) {
    const string configFile = EXAMPLES_PATH + "basic/simulation.yaml";

    Application app;

    app.LoadConfigFromFile(configFile);

    app.PrintConfig();

    app.UserInitialization();
}