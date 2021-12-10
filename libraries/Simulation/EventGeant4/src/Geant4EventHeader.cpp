//
// Created by lobis on 03/12/2021.
//

#include "Geant4EventHeader.h"

#include <spdlog/spdlog.h>

using namespace std;

ClassImp(Geant4EventHeader);

void Geant4EventHeader::Print() const {
    spdlog::info("Geant4EventHeader::Print - Header timestamp: {0.2f}", fTimestamp);

    fSimulationConfig->Print();

    fGeant4GeometryInfo->Print();
}
