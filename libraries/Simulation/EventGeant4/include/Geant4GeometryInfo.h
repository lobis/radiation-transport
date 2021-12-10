//
// Created by lobis on 03/12/2021.
//

#ifndef RADIATION_TRANSPORT_Geant4GeometryInfo_H
#define RADIATION_TRANSPORT_Geant4GeometryInfo_H

#include <TString.h>
#include <TVector3.h>

#include <map>
#include <set>
#include <vector>

#include "Geant4GeometryInfo.h"

class G4VPhysicalVolume;

class Geant4GeometryInfo {
    ClassDef(Geant4GeometryInfo, 1);

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
    inline Geant4GeometryInfo() = default;

    void PopulateFromGdml(const TString&);

    TString GetAlternativeNameFromGeant4PhysicalName(const TString&) const;

    Int_t GetIDFromVolumeName(const TString&) const;

    void PopulateFromGeant4World(const G4VPhysicalVolume*);

    inline std::vector<TString> GetAllPhysicalVolumes() const { return fPhysicalVolumes; }
    inline std::vector<TString> GetAllLogicalVolumes() const { return fLogicalVolumes; }

    inline bool IsValidPhysicalVolume(const TString& volume) const { return fPhysicalToLogicalVolumeMap.count(volume) > 0; }
    inline bool IsValidLogicalVolume(const TString& volume) const { return fLogicalToPhysicalMap.count(volume) > 0; }
    inline std::vector<TString> GetAllPhysicalVolumesFromLogical(const TString& logicalVolume) const {
        return fLogicalToPhysicalMap.at(logicalVolume);
    }

    void Print() const;
};

#endif  // RADIATION_TRANSPORT_Geant4GeometryInfo_H
