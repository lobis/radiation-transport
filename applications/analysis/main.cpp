//
// Created by lobis on 26/11/2021.
//

#include <DataEvent.h>
#include <SimulationGeometryInfo.h>
#include <TApplication.h>
#include <TBrowser.h>
#include <TFile.h>
#include <TROOT.h>
#include <TTree.h>
#include <spdlog/spdlog.h>

#include <ROOT/RDataFrame.hxx>
#include <iostream>

#include "SimulationToVetoProcess.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc > 2) {
        spdlog::error("Application opened with bad number of arguments. Only one optional argument (config file)");
        exit(1);
    }
    TString filename = "";
    if (argc == 2) {
        filename = argv[1];
        spdlog::info("Application opened with config file as argument: '{}'", filename);
    }

    TApplication app("Analysis", &argc, argv);

    spdlog::info("Starting Analysis");

    TFile fFile("/tmp/tmp.eYzASlam4v/cmake-build-docker/applications/simulation/examples/iaxo/babyIAXO.root");
    TTree* fEventTree = fFile.Get<TTree>("EventTree");

    DataEvent* fEvent = nullptr;

    ROOT::EnableImplicitMT(4);

    fEventTree->SetBranchAddress("fEvent", &fEvent);

    SimulationToVetoProcess process;

    TFile file("/tmp/output.root", "RECREATE");

    VetoEvent output;
    output.fEventID = 2;
    TTree outputTree("OutputVeto", "Output Veto");
    int d = 1;
    outputTree.Branch("fVetoEvent", &output);
    outputTree.Branch("xxx", &d);

    for (int i = 0; i < fEventTree->GetEntries(); i++) {
        fEventTree->GetEntry(i);
        spdlog::info("{}", i);
        auto outputVeto = process.Process(*fEvent);
        output = outputVeto;
        outputTree.Fill();
    }

    outputTree.Write();

    file.Close();
    // app.Run();

    return 0;
}