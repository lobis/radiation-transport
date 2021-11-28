//
// Created by lobis on 28/11/2021.
//

#ifndef RADIATION_TRANSPORT_SIMULATIONTOVETOPROCESS_H
#define RADIATION_TRANSPORT_SIMULATIONTOVETOPROCESS_H

#include <DataEvent.h>
#include <TString.h>
#include <VetoEvent.h>

#include <set>

class SimulationToVetoProcess {
   public:
    std::set<TString> fVetoVolumes;
    std::map<TString, size_t> fVetoNameToIDMap;

    VetoEvent Process(const DataEvent&);

    inline void Print() const {}

    inline SimulationToVetoProcess() = default;

    void AddGeometryInfo(const SimulationGeometryInfo&);

   private:
    // ClassDef(SimulationToVetoProcess, 1);
};

#endif  // RADIATION_TRANSPORT_SIMULATIONTOVETOPROCESS_H
