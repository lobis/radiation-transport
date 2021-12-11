//
// Created by lobis on 30/11/2021.
//

#include <TFile.h>
#include <TROOT.h>
#include <TTree.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <filesystem>

#include "Geant4EventHeader.h"

using namespace std;

namespace fs = std::filesystem;

TEST(EventGeant4, Dictionary) {
    EXPECT_TRUE(TClass::GetClass("ThisClassDoesNotExist") == nullptr);
    for (const auto& className : {"Geant4Event", "Geant4Track", "Geant4Hits", "Geant4EventHeader", "Geant4GeometryInfo"}) {
        spdlog::info("Checking existence of dictionary for: {}", className);
        EXPECT_TRUE(TClass::GetClass(className) != nullptr);
    }
}

/*
TEST(EventGeant4, RootFile) {
    const TString& filename = "/tmp/test.root";

    TFile file(filename, "RECREATE");

    Geant4EventHeader header;

    header.fGeant4GeometryInfo = new Geant4GeometryInfo();
    header.fSimulationConfig = new SimulationConfig();

    TTree tree("EventTree", "EventTree");

    tree.Branch("fEventHeader", &header);

    tree.Fill();

    file.Write();
    file.Close();

    // READ
    TFile fileRead(filename);
    fileRead.ls();

    auto treeRead = fileRead.Get<TTree>("EventTree");

    treeRead->Print();

    Geant4EventHeader* headerRead;

    treeRead->SetBranchAddress("fEventHeader", &headerRead);

    treeRead->GetEntry(0);

    EXPECT_TRUE(headerRead != nullptr);

    treeRead->Show(0);

    headerRead->Print();
}
*/