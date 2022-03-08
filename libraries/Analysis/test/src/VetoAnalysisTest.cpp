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
#include "VetoAnalysisProcess.h"
#include "VetoEvent.h"
#include "spdlog/spdlog.h"

#ifdef REST_INSTALLED
#include "RestSignalProcess.h";
#endif

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
    config.fNumberOfEvents = 10000;

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
    const fs::path simulationFile = "/tmp/tmp.wPkPjmf9YO/cmake-build-sultan2/libraries/Analysis/test/muons.root";  // TEST_PATH + "muons.root";

    const fs::path analysisFile = simulationFile.parent_path() / ("analysis." + (string)simulationFile.filename());

    spdlog::info("Simulation file: {}", string(simulationFile));

    TFile inputFile(simulationFile.c_str(), "READ");

    TTree* eventTree = inputFile.Get<TTree>("EventTree");

    auto header = dynamic_cast<Geant4EventHeader*>(eventTree->GetUserInfo()->At(0));
    auto geometryInfo = header->GetGeant4GeometryInfo();

    geometryInfo->Print();

    Geant4Event* event = nullptr;
    eventTree->SetBranchAddress("fEvent", &event);

    TFile outputFile(analysisFile.c_str(), "RECREATE");
    spdlog::info("Output file: {}", outputFile.GetName());

    auto analysisTree = new TTree("AnalysisTree", "AnalysisTree");

    VetoEvent* vetoEvent = nullptr;
    analysisTree->Branch("fVetoEvent", &vetoEvent);

    auto analysisProcess = VetoAnalysisProcess();
    analysisProcess.SetGeometryInfo(*geometryInfo);

    analysisProcess.SetVetoInformation("scintillatorVolume", "scintillatorLightGuideVolume", 65.0);

    const auto n = eventTree->GetEntries();
    for (int i = 0; i < n; i++) {
        eventTree->GetEntry(i);
        spdlog::info("\t- {} - EventID {}", i, event->fEventID);

        *vetoEvent = analysisProcess.SimulationEventToVetoEvent(*event);

        spdlog::info("n: {}", vetoEvent->fN);

        analysisTree->Fill();
    }

    analysisTree->Write();
    outputFile.Close();
}

#ifdef REST_INSTALLED
TEST(VetoAnalysis, RestSignal) { auto signalEvent = new TRestDetectorSignalEvent(); }
#endif