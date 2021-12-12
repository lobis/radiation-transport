#include <TFile.h>
#include <TTree.h>

int validate(const TString& filename = "exampleSingleVetoMuonsFromDisk.root") {
    gSystem->Load("libSimulation.so");

    TFile file(filename);

    file.ls();

    auto EventTree = file.Get<TTree>("EventTree");

    EventTree->Print();

    Geant4Event* fEvent = nullptr;

    EventTree->SetBranchAddress("fEvent", &fEvent);

    cout << "Event Tree has " << EventTree->GetEntries() << " events" << endl;

    double averageEnergy = 0;
    for (int i = 0; i < EventTree->GetEntries(); i++) {
        EventTree->GetEntry(i);
        averageEnergy += fEvent->fSensitiveVolumesTotalEnergy / EventTree->GetEntries();
    }

    cout << "Average energy is " << averageEnergy << " keV" << endl;

    if (averageEnergy <= 13500 && averageEnergy > 12500) {
        cout << "Average energy within range" << endl;
        return 0;
    }

    cout << "Average energy out of range" << endl;

    return 1;
}