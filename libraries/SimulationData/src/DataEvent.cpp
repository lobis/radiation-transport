//
// Created by lobis on 23/11/2021.
//

#include "DataEvent.h"

#include <spdlog/spdlog.h>

ClassImp(DataEvent);

using namespace std;

void DataEvent::Print() const { spdlog::info("Not implemented yet!"); }

void DataEvent::PrintSensitiveInfo() const {
    spdlog::info("DataEvent::PrintSensitiveInfo - EventID: {} - SubEventID: {}", fEventID, fSubEventID);
    for (int i = 0; i < fSensitiveVolumeEnergy.size(); i++) {
        spdlog::info("Sensitive Detector: {} (ID {}) E = {} keV", fSensitiveVolumeName[i], fSensitiveVolumeID[i], fSensitiveVolumeEnergy[i]);
    }
}
