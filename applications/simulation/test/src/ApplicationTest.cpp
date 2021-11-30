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

    app.UserInitialization();

    app.Initialize();

    TFile file(config.GetOutputFileAbsolutePath().c_str());

    file.ls();

    TTree* tree = file.Get<TTree>("EventTree");

    EXPECT_TRUE(tree != nullptr);

    spdlog::info("Tree entries: {}", tree->GetEntries());

    tree->Print();

    Double_t averageEnergy = 0;
    DataEvent* event = nullptr;
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

    app.UserInitialization();

    app.Initialize();

    TFile file(config.GetOutputFileAbsolutePath().c_str());

    TTree* tree = file.Get<TTree>("EventTree");

    tree->Print();

    spdlog::info("Tree entries: {}", tree->GetEntries());
    Double_t totalEnergy = 0;
    DataEvent* event = nullptr;
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
    app.UserInitialization();
    app.Initialize();

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
    app.UserInitialization();
    app.Initialize();

    TFile file(config.GetOutputFileAbsolutePath().c_str());
    TTree* tree = file.Get<TTree>("EventTree");

    EXPECT_EQ(300, tree->GetEntries());
}
/*
TEST(Application, SerialVsMTSameResults) {
    const string configFile = EXAMPLES_PATH + "basic/simulation.yaml";

    auto configOriginal = SimulationConfig(configFile);

    auto configSerial = configOriginal;
    configSerial.fRunManagerType = "serial";
    configSerial.fOutputFilename = "/tmp/test.serial.root";

    auto configMT = configOriginal;
    configSerial.fRunManagerType = "multithreading";
    configSerial.fOutputFilename = "/tmp/test.mt.root";

    vector<Double_t> energySerial;
    vector<Double_t> energyMT;

    for (const auto& config : {configSerial, configMT}) {
        Application app;

        app.LoadConfigFromFile(config);
        app.UserInitialization();
        app.Initialize();

        TFile file(config.fOutputFilename.c_str());

        TTree* tree = file.Get<TTree>("EventTree");
        DataEvent* event = nullptr;
        tree->SetBranchAddress("fEvent", &event);
        for (int i = 0; i < tree->GetEntries(); i++) {
            tree->GetEntry(i);
            if (config.fRunManagerType == "serial") {
                energySerial.emplace_back(event->fSensitiveVolumesTotalEnergy);
            } else {
                energyMT.emplace_back(event->fSensitiveVolumesTotalEnergy);
            }
            event->PrintSensitiveInfo();
        }
    }

    sort(energySerial.begin(), energySerial.end());
    sort(energyMT.begin(), energyMT.end());

    EXPECT_EQ(energySerial, energyMT);
}
 */