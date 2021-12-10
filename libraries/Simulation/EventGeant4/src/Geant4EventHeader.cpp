//
// Created by lobis on 03/12/2021.
//

#include "Geant4EventHeader.h"

#include <spdlog/spdlog.h>

using namespace std;

ClassImp(Geant4EventHeader);

void Geant4EventHeader::Print() const {
    spdlog::info("Geant4EventHeader::Print - Header timestamp: {}", fTimestamp);

    if (fSimulationConfig) {
        fSimulationConfig->Print();
    } else {
        spdlog::info("No SimulationConfig stored");
    }

    if (fGeant4GeometryInfo) {
        fGeant4GeometryInfo->Print();
    } else {
        spdlog::info("No Geant4GeometryInfo stored");
    }
}
