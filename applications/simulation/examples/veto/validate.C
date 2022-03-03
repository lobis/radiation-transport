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
    double minEnergy = 0;
    double maxEnergy = 0;

    double averagePrimaryEnergy = 0;
    double minPrimaryEnergy = 0;
    double maxPrimaryEnergy = 0;

    for (int i = 0; i < EventTree->GetEntries(); i++) {
        EventTree->GetEntry(i);
        auto energy = fEvent->fSensitiveVolumesTotalEnergy;
        averageEnergy += energy / EventTree->GetEntries();

        if (energy < minEnergy || i == 0) {
            minEnergy = energy;
        }
        if (energy > maxEnergy) {
            maxEnergy = energy;
        }

        auto primaryEnergy = fEvent->fPrimaryEnergy;
        averagePrimaryEnergy += primaryEnergy / EventTree->GetEntries();

        if (primaryEnergy < minPrimaryEnergy || i == 0) {
            minPrimaryEnergy = primaryEnergy;
        }
        if (primaryEnergy > maxPrimaryEnergy) {
            maxPrimaryEnergy = primaryEnergy;
        }
    }

    cout << "Average sensitive energy is " << averageEnergy << " keV" << endl;
    cout << "Min / Max sensitive energy is " << minEnergy << " / " << maxEnergy << " keV" << endl;

    cout << "Average primary energy is " << averagePrimaryEnergy << " keV" << endl;
    cout << "Min / Max primary energy is " << minPrimaryEnergy << " / " << maxPrimaryEnergy << " keV" << endl;

    if (averageEnergy <= 12500 && averageEnergy > 10500) {
        cout << "Average energy within range" << endl;
        return 0;
    }

    cout << "Average energy out of range" << endl;

    return 1;
}