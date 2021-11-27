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

    DataEvent fEvent;
    auto pEvent = &fEvent;

    ROOT::EnableImplicitMT(4);

    fEventTree->SetBranchAddress("fEvent", &pEvent);

    for (int i = 0; i < fEventTree->GetEntries(); i++) {
        fEventTree->GetEntry(i);
        cout << fEvent.fSimulationGeometryInfo << endl;
        spdlog::info("event id: {}, energy: {}", fEvent.fEventID, fEvent.fSensitiveVolumesTotalEnergy);
    }

    fEventTree->Draw("fEvent.fSensitiveVolumesTotalEnergy");

    app.Run();

    return 0;
}