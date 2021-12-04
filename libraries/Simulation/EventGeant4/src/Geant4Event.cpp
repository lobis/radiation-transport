//
// Created by lobis on 23/11/2021.
//

#include "Geant4Event.h"

#include <spdlog/spdlog.h>

ClassImp(Geant4Event);

using namespace std;

void Geant4Event::Print() const { spdlog::info("Not implemented yet!"); }

void Geant4Event::PrintSensitiveInfo() const {
    spdlog::info("Geant4Event::PrintSensitiveInfo - EventID: {} - SubEventID: {}", fEventID, fSubEventID);
    for (int i = 0; i < fSensitiveVolumeEnergy.size(); i++) {
        spdlog::info("Sensitive Detector: {} (ID {}) E = {} keV", fSensitiveVolumeName[i], fSensitiveVolumeID[i], fSensitiveVolumeEnergy[i]);
    }
}
