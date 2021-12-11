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

TEST(Application, ExampleIAXO) {
    // TODO: NOT FINISHED YET! compare energies with standard simulation

    const string configFile = EXAMPLES_PATH + "iaxo/neutrons.simulation.yaml";

    auto config = SimulationConfig(configFile);

    config.fSeed = 99;
    config.fRunManagerType = "serial";
    config.fFullChain = false;
    config.fNumberOfEvents = 100;

    config.fSourceConfig.fPositionDistributionType = "point";
    config.fSourceConfig.fPositionDistributionCenter = {0, 1500, 0};
    config.fSourceConfig.fAngularDistributionType = "flux";
    config.fSourceConfig.fAngularDistributionDirection = {0, -1, 0};

    config.fDetectorConfig.fKeepOnlyTracksInTheseVolumes = true;
    config.fDetectorConfig.fKeepOnlyTracksInTheseVolumesList = {"gasVolume", "^scintillatorVolume"};

    config.Print();

    Application app;
    app.LoadConfigFromFile(config);
    app.PrintConfig();
    app.Run();

    TFile file(config.GetOutputFileAbsolutePath().c_str());
    auto filenameNoReduction = config.GetOutputFileAbsolutePath();

    spdlog::info("The size of {} is {:0.2f} MB", filenameNoReduction, std::filesystem::file_size(filenameNoReduction) / 1E6);

    file.ls();

    TTree* tree = file.Get<TTree>("EventTree");
    Geant4Event* event = nullptr;
    tree->SetBranchAddress("fEvent", &event);

    for (int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        event->PrintSensitiveInfo();
    }

    event->fEventHeader->Print();
}
