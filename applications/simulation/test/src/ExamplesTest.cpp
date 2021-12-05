//
// Created by lobis on 11/22/2021.
//

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <vector>

#include "Application.h"
#include "Exceptions.h"

#define EXAMPLES_PATH string(std::filesystem::path(__FILE__).parent_path().parent_path().parent_path()) + "/examples/"

using namespace std;

TEST(Application, ExampleVeto) {
    const string configFile = EXAMPLES_PATH + "veto/simulation.yaml";

    auto config = SimulationConfig(configFile);
    config.fRunManagerType = "serial";

    Application app;

    app.LoadConfigFromFile(config);

    app.PrintConfig();

    app.Run();

    TFile file(config.GetOutputFileAbsolutePath().c_str());

    file.ls();

    TTree* tree = file.Get<TTree>("EventTree");

    EXPECT_TRUE(tree != nullptr);

    spdlog::info("Tree entries: {}", tree->GetEntries());

    // Run the validation macro
    TString macro = EXAMPLES_PATH + "veto/validate.C";
    gROOT->ProcessLine(TString::Format(".L %s", macro.Data()));  // Load macro
    int result = gROOT->ProcessLine(TString::Format("validate(\"%s\")", file.GetName()));

    EXPECT_EQ(result, 0);
}
