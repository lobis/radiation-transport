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

TEST(Application, LoadGeometryGdml) {
    const string configFile = EXAMPLES_PATH + "basic/simulation.yaml";

    Application app;

    app.LoadConfigFromFile(configFile);

    app.PrintConfig();

    app.UserInitialization();
}

TEST(Application, BadPhysicsList) {
    SimulationConfig config(EXAMPLES_PATH + "basic/simulation.yaml");

    config.fPhysicsListConfig.fPhysicsLists.clear();
    config.fPhysicsListConfig.fPhysicsLists.emplace_back("ThisPhysicsListNameDoesNotExist");

    Application app;

    app.LoadConfigFromFile(config);

    app.PrintConfig();

    try {
        app.UserInitialization();
    } catch (exception& ex) {
        const auto targetException = (exception*)&exceptions::BadPhysicsList;
        EXPECT_STREQ(ex.what(), targetException->what());
    }
}

TEST(Application, FullRunSerial) {
    const string configFile = EXAMPLES_PATH + "basic/simulation.yaml";

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

    tree->Print();

    Double_t averageEnergy = 0;
    Geant4Event* event = nullptr;
    tree->SetBranchAddress("fEvent", &event);
    for (int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        spdlog::info("\t-{} - EventID {}", i, event->fEventID);
        averageEnergy += event->fSensitiveVolumesTotalEnergy / tree->GetEntries();
    }

    spdlog::warn("Serial run average energy: {} keV", averageEnergy);
}

TEST(Application, FullRunMT) {
    const string configFile = EXAMPLES_PATH + "basic/simulation.yaml";

    auto config = SimulationConfig(configFile);
    config.fRunManagerType = "multithreading";

    Application app;

    app.LoadConfigFromFile(config);

    app.PrintConfig();

    app.Run();

    TFile file(config.GetOutputFileAbsolutePath().c_str());

    TTree* tree = file.Get<TTree>("EventTree");

    tree->Print();

    spdlog::info("Tree entries: {}", tree->GetEntries());
    Double_t totalEnergy = 0;
    Geant4Event* event = nullptr;
    tree->SetBranchAddress("fEvent", &event);
    for (int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        spdlog::info("\t-{} - EventID {}", i, event->fEventID);
        totalEnergy += event->fSensitiveVolumesTotalEnergy;
    }

    spdlog::warn("Multithreading run total energy: {} keV", totalEnergy);
}

TEST(Application, DecayFullChainOff) {
    const string configFile = EXAMPLES_PATH + "decay/simulation.yaml";

    auto config = SimulationConfig(configFile);
    config.fSeed = 1;
    config.fRunManagerType = "serial";
    config.fFullChain = false;

    Application app;
    app.LoadConfigFromFile(config);
    app.PrintConfig();
    app.Run();

    TFile file(config.GetOutputFileAbsolutePath().c_str());
    TTree* tree = file.Get<TTree>("EventTree");

    EXPECT_EQ(100, tree->GetEntries());
}

TEST(Application, DecayFullChainOn) {
    const string configFile = EXAMPLES_PATH + "decay/simulation.yaml";

    auto config = SimulationConfig(configFile);
    config.fSeed = 1;
    config.fRunManagerType = "serial";
    config.fFullChain = true;

    Application app;
    app.LoadConfigFromFile(config);
    app.PrintConfig();
    app.Run();

    TFile file(config.GetOutputFileAbsolutePath().c_str());
    TTree* tree = file.Get<TTree>("EventTree");

    EXPECT_EQ(300, tree->GetEntries());
}

TEST(Application, RemoveUnwantedTracks) {
    // TODO: NOT FINISHED YET! compare energies with standard simulation

    const string configFile = EXAMPLES_PATH + "iaxo/neutrons.simulation.yaml";

    auto config = SimulationConfig(configFile);

    config.fSeed = 100;
    config.fRunManagerType = "serial";
    config.fFullChain = false;
    config.fNumberOfEvents = 100;

    config.fSourceConfig.fPositionDistributionType = "point";
    config.fSourceConfig.fPositionDistributionCenter = {0, 1500, 0};
    config.fSourceConfig.fAngularDistributionType = "flux";
    config.fSourceConfig.fAngularDistributionDirection = {0, -1, 0};

    config.fDetectorConfig.fKeepOnlyTracksInTheseVolumesList = {"gasVolume", "scintillatorVolume-800.0mm"};

    Application app;
    app.LoadConfigFromFile(config);
    app.PrintConfig();
    app.Run();

    TFile file(config.GetOutputFileAbsolutePath().c_str());
    auto filenameNoReduction = config.GetOutputFileAbsolutePath();

    spdlog::info("The size of {} is {:0.2f} MB", filenameNoReduction, std::filesystem::file_size(filenameNoReduction) / 1E6);

    TTree* tree = file.Get<TTree>("EventTree");
    Geant4Event* event = nullptr;
    tree->SetBranchAddress("fEvent", &event);
    for (int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        event->PrintSensitiveInfo();
    }
}
