//
// Created by lobis on 11/22/2021.
//

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <filesystem>

#include "Application.h"

using namespace std;

auto examplesPath = string(std::filesystem::path(__FILE__).parent_path().parent_path().parent_path()) + "/examples/";

TEST(ApplicationTest, ShowUsage) {
    Application app;
    app.ShowUsage();
}

TEST(ApplicationTest, LoadConfigFromFile) {
    const string exampleConfig = examplesPath + "basic/simulation.yaml";

    Application app;

    app.LoadConfigFromFile(exampleConfig);

    app.PrintConfig();

    app.Run();
}
