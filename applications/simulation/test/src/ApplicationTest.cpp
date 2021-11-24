//
// Created by lobis on 11/22/2021.
//

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <filesystem>

#include "Application.h"
// #include "Exceptions.h"

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

    app.Run();
}

TEST(ApplicationTest, NoGeometryFile) {
    SimulationConfig config(examplesPath + "basic/simulation.yaml");

    config.fGeometryFilename = "";

    Application app;

    app.LoadConfigFromFile(config);

    app.PrintConfig();

    try {
        app.Run();
    } catch (exception& ex) {
        // const auto testException = (exception*)&exceptions::NoGeometryFile;
        // EXPECT_STREQ(ex.what(), testException->what());
    }
}