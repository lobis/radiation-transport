//
// Created by lobis on 03/12/2021.
//

#ifndef RADIATION_TRANSPORT_DATAEVENTHEADER_H
#define RADIATION_TRANSPORT_DATAEVENTHEADER_H

#include <SimulationConfig.h>
#include <TString.h>

#include <map>
#include <vector>

#include "SimulationGeometryInfo.h"

class DataEventHeader {
    ClassDef(DataEventHeader, 1);

   public:
    inline DataEventHeader() = default;

    SimulationConfig fSimulationConfig;  // configuration used in simulation
    SimulationGeometryInfo fSimulationGeometryInfo;
};

#endif  // RADIATION_TRANSPORT_DATAEVENTHEADER_H
