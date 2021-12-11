//
// Created by lobis on 11/24/2021.
//

#ifndef RADIATION_TRANSPORT_DETECTORCONSTRUCTION_H
#define RADIATION_TRANSPORT_DETECTORCONSTRUCTION_H

#include <DetectorConstructionConfig.h>

#include <G4VUserDetectorConstruction.hh>
#include <filesystem>
#include <globals.hh>

class G4VPhysicalVolume;
class G4GDMLParser;

class DetectorConstruction : public G4VUserDetectorConstruction {
   public:
    DetectorConstruction(const DetectorConstructionConfig&);
    ~DetectorConstruction();

    void PrintGeometryInfo();
    G4VPhysicalVolume* Construct() override;

    void ConstructSDandField() override;

    inline G4VPhysicalVolume* GetWorld() const { return fWorld; }

    bool CheckOverlaps() const;

    std::vector<G4LogicalVolume*> GetAllLogicalVolumes() const;

   private:
    DetectorConstruction();

    const std::filesystem::path fGeometryFilename;

    G4VPhysicalVolume* fWorld = nullptr;
};

#endif  // RADIATION_TRANSPORT_DETECTORCONSTRUCTION_H
