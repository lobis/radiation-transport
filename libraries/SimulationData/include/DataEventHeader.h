//
// Created by lobis on 03/12/2021.
//

#ifndef RADIATION_TRANSPORT_DATAEVENTHEADER_H
#define RADIATION_TRANSPORT_DATAEVENTHEADER_H

#include <SimulationConfig.h>
#include <TString.h>

#include <map>
#include <vector>

class G4VPhysicalVolume;

class SimulationGeometryInfo {
   public:
    std::vector<TString> fGdmlNewPhysicalNames;
    std::map<TString, TString> fGeant4PhysicalNameToNewPhysicalNameMap; /*
                                                                         * only makes sense when using assembly
                                                                         */
    std::vector<TString> fPhysicalVolumes;                              /* Geant4 physical names */
    std::vector<TString> fLogicalVolumes;
    std::vector<TString> fMaterials;

    std::map<TString, TString> fPhysicalToLogicalVolumeMap;
    std::map<TString, std::vector<TString> > fLogicalToPhysicalMap;
    // many physical volumes can point to one single logical
    std::map<TString, TString> fLogicalToMaterialMap;
    std::map<TString, TVector3> fPhysicalToPositionInWorldMap;

   public:
    inline SimulationGeometryInfo() = default;

    void PopulateFromGdml(const TString&);

    TString GetAlternativeNameFromGeant4PhysicalName(const TString&) const;

    Int_t GetIDFromVolumeName(const TString&) const;

    void PopulateFromGeant4World(const G4VPhysicalVolume*);
};

class DataEventHeader {
    ClassDef(DataEventHeader, 1);

   public:
    SimulationConfig fSimulationConfig;  // configuration used in simulation
    SimulationGeometryInfo fSimulationGeometryInfo;
};

#endif  // RADIATION_TRANSPORT_DATAEVENTHEADER_H
