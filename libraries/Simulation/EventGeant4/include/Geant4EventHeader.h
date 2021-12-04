//
// Created by lobis on 03/12/2021.
//

#ifndef RADIATION_TRANSPORT_Geant4EventHEADER_H
#define RADIATION_TRANSPORT_Geant4EventHEADER_H

#include <TString.h>

#include <map>
#include <vector>

#include "Geant4GeometryInfo.h"

class Geant4EventHeader {
    ClassDef(Geant4EventHeader, 1);

   public:
    inline Geant4EventHeader() = default;

    void Print() const;

    Geant4GeometryInfo* fGeant4GeometryInfo;

    inline Geant4GeometryInfo* GetGeant4GeometryInfo() const { return fGeant4GeometryInfo; }
};

#endif  // RADIATION_TRANSPORT_Geant4EventHEADER_H
