//
// Created by lobis on 27/11/2021.
//

#include "VetoEvent.h"

#include <spdlog/spdlog.h>

ClassImp(VetoEvent);

void VetoEvent::Print() const {
    spdlog::info("Event ID: {} - SubEventID: {} - Number of registered veto events: {}", fEventID, fSubEventID, fNumberOfVetoes);

    for (int i = 0; i < fNumberOfVetoes; i++) {
        spdlog::info("\t- Veto ID {} - E = {} keV", fVetoID[i], fVetoRawEnergy[i]);
    }
}