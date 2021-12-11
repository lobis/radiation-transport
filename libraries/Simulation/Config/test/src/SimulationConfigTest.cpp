//
// Created by lobis on 11/22/2021.
//

#include <TFile.h>
#include <TROOT.h>
#include <TTree.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <filesystem>

#include "SimulationConfig.h"

using namespace std;

#define BASE_PATH string(std::filesystem::path(__FILE__).parent_path().parent_path()) + "/"

TEST(Config, Dictionary) {
    EXPECT_TRUE(TClass::GetClass("ThisClassDoesNotExist") == nullptr);
    for (const auto& className : {"SimulationConfig", "SourceConfig"}) {
        spdlog::info("Checking existence of dictionary for: {}", className);
        EXPECT_TRUE(TClass::GetClass(className) != nullptr);
    }
}
/*
TEST(Config, RootFile) {
    const TString& filename = "/tmp/test.root";
    std::filesystem::remove(filename.Data());

    TFile file(filename, "CREATE");

    auto simulationConfig = new SimulationConfig();

    TTree tree("ConfigTree", "ConfigTree");

    tree.Branch("fSimulationConfig", &simulationConfig);

    tree.Fill();

    file.Write();
    file.Close();

    // READ
    TFile fileRead(filename);
    fileRead.ls();

    TTree* treeRead = fileRead.Get<TTree>("ConfigTree");

    spdlog::info("Config Tree entries: {}", treeRead->GetEntries());

    treeRead->Print();

    SimulationConfig* simulationConfigRead;

    treeRead->SetBranchAddress("fSimulationConfig", &simulationConfigRead);

    treeRead->GetEntry(0);

    EXPECT_TRUE(simulationConfigRead != nullptr);

    simulationConfigRead->Print();
}
*/

TEST(SimulationConfig, LoadFromFile) {
    const auto config = SimulationConfig(BASE_PATH + "files/simulation.yaml");

    EXPECT_EQ(config.fRunManagerType, "serial");
}

TEST(SimulationConfig, DefaultConfig) {
    const SimulationConfig config;

    EXPECT_EQ(config.fVerboseLevel, "info");
    EXPECT_EQ(config.fRunManagerType, "serial");
    EXPECT_EQ(config.fThreads, 2);
    EXPECT_EQ(config.fSeed, 0);
    EXPECT_EQ(config.fCommands.empty(), true);
}