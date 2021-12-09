//
// Created by lobis on 23/11/2021.
//

#include "Geant4Track.h"

#include <spdlog/spdlog.h>

ClassImp(Geant4Track);

using namespace std;

void Geant4Track::Print() const { spdlog::info("Not implemented yet!"); }

double Geant4Track::GetEnergyInVolume(const TString& volume) {
    if (fEnergyInVolumeMap.empty()) {
        InitializeEnergyInVolumeMap();
    }
    if (fEnergyInVolumeMap.count(volume.Data()) > 0) {
        return fEnergyInVolumeMap.at(volume.Data());
    }
    return 0;
}

void Geant4Track::InitializeEnergyInVolumeMap() {
    fEnergyInVolumeMap.clear();
    for (int i = 0; i < fSteps.fN; i++) {
        string volume = fSteps.fVolumeName[i].Data();
        double energy = fSteps.fEnergy[i];
        fEnergyInVolumeMap[volume] = fEnergyInVolumeMap[volume] + energy;
    }
}
