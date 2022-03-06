//
// Created by lobis on 30/11/2021.
//

#include <TFile.h>
#include <TROOT.h>
#include <TTree.h>
#include <gtest/gtest.h>

#include <filesystem>

#include "Application.h"
#include "Geant4EventHeader.h"
#include "SimulationConfig.h"
#include "VetoAnalysis.h"
#include "spdlog/spdlog.h"

using namespace std;

namespace fs = std::filesystem;

#define EXAMPLES_PATH \
    string(fs::path(__FILE__).parent_path().parent_path().parent_path().parent_path().parent_path()) + "/applications/simulation/examples/"

#define TEST_PATH string(fs::path(__FILE__).parent_path().parent_path()) + "/"

TEST(VetoAnalysis, GenerateData) {
    spdlog::info(EXAMPLES_PATH);

    const string configFile = EXAMPLES_PATH + "iaxo/muons.simulation.yaml";

    auto config = SimulationConfig(configFile);
    config.fOutputFilename = "muons.root";
    config.fSeed = 1000;
    config.fNumberOfEvents = 5000;

    Application app;

    app.LoadConfigFromFile(config);

    app.PrintConfig();

    app.Run();

    const auto simulationFileName = config.GetOutputFileAbsolutePath();
    spdlog::info("Simulation file: {}", simulationFileName);

    TFile file(simulationFileName.c_str());

    TTree* tree = file.Get<TTree>("EventTree");

    tree->Print();
}

TEST(VetoAnalysis, AnalyseData) {
    const string simulationFileName = "/tmp/tmp.wPkPjmf9YO/cmake-build-sultan2/libraries/Analysis/test/muons.root";  // TEST_PATH + "muons.root";
    spdlog::info("Simulation file: {}", simulationFileName);

    TFile file(simulationFileName.c_str());

    TTree* tree = file.Get<TTree>("EventTree");

    Geant4Event* event = nullptr;
    tree->SetBranchAddress("fEvent", &event);
    for (int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        spdlog::info("\t- {} - EventID {}", i, event->fEventID);

        // auto VetoEvent = new VetoAnalysis::VetoEvent();
        auto vetoEvent = VetoAnalysis::SimulationEventToVetoEvent(*event);
    }
}