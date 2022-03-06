//
// Created by lobis on 3/6/2022.
//

#ifndef RADIATION_TRANSPORT_VETOANALYSIS_H
#define RADIATION_TRANSPORT_VETOANALYSIS_H

#include "Geant4Event.h"

namespace VetoAnalysis {
class VetoEvent {};

VetoEvent SimulationEventToVetoEvent(const Geant4Event& input);

}  // namespace VetoAnalysis

#endif  // RADIATION_TRANSPORT_VETOANALYSIS_H
