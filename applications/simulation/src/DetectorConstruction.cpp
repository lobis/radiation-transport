//
// Created by lobis on 11/24/2021.
//

#include "DetectorConstruction.h"

#include <TString.h>

#include <G4GDMLParser.hh>
#include <G4LogicalVolume.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4SDManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4VPhysicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4tgbDetectorBuilder.hh>
#include <G4tgbVolumeMgr.hh>
#include <G4tgrFileReader.hh>
#include <G4tgrLineProcessor.hh>
#include <G4tgrVolumeMgr.hh>
#include <cstdio>
#include <fstream>

#include "spdlog/spdlog.h"

using namespace std;

namespace fs = std::filesystem;

DetectorConstruction::DetectorConstruction() = default;

DetectorConstruction::DetectorConstruction(const fs::path& geometryFilename) : fGeometryFilename(geometryFilename) {
    spdlog::info("DetectorConstruction::DetectorConstruction - filename: '{}'", geometryFilename.c_str());
    if (string(fGeometryFilename).empty()) {
        spdlog::error("Geometry file not defined in DetectorConstruction");
        exit(1);
    }
    if (!fs::exists(fGeometryFilename)) {
        spdlog::error("Geometry file '{}' not found", fGeometryFilename.c_str());
        exit(1);
    }
}

DetectorConstruction::~DetectorConstruction() = default;

G4VPhysicalVolume* DetectorConstruction::Construct() {
    spdlog::debug("DetectorConstruction::Construct");
    spdlog::info("DetectorConstruction::Construct - Reading geometry from '{}'", fGeometryFilename.c_str());

    // read extension
    if (fGeometryFilename.extension() == ".gdml") {
        spdlog::debug("Reading geometry as GDML");
        G4GDMLParser parser;
        parser.Read(fGeometryFilename.c_str(), true);
        fWorld = parser.GetWorldVolume();
    } else {
        spdlog::debug("Reading geometry as TG (Text Geometry)");
        // read detector
        G4tgbVolumeMgr* tgbVolumeManager = G4tgbVolumeMgr::GetInstance();
        tgbVolumeManager->AddTextFile(fGeometryFilename.c_str());
        auto detectorBuilder = new G4tgbDetectorBuilder();

        G4tgrFileReader* reader = G4tgrFileReader::GetInstance();
        auto lineProcessor = new G4tgrLineProcessor();
        reader->SetLineProcessor(lineProcessor);
        reader->ReadFiles();

        G4tgrVolumeMgr* tgrVolumeManager = G4tgrVolumeMgr::GetInstance();
        const G4tgrVolume* world = tgrVolumeManager->GetTopVolume();
        // construct detector
        fWorld = detectorBuilder->ConstructDetector(world);

        delete lineProcessor;
        delete detectorBuilder;
    }

    PrintGeometryInfo();

    return fWorld;
}

void DetectorConstruction::ConstructSDandField() {}

void DetectorConstruction::PrintGeometryInfo() {
    spdlog::info("DetectorConstruction::PrintGeometryInfo - Begin");
    const int n = int(fWorld->GetLogicalVolume()->GetNoDaughters());
    for (int i = 0; i < n; i++) {
        G4VPhysicalVolume* volume = fWorld->GetLogicalVolume()->GetDaughter(i);
        auto namePhysical = volume->GetName();
        auto nameLogical = volume->GetLogicalVolume()->GetName();
        auto nameMaterial = volume->GetLogicalVolume()->GetMaterial()->GetName();
        auto position = volume->GetTranslation();
        spdlog::info("---> {} - physical: {} - logical: {} - material: {} - position: ({:03.2f}, {:03.2f}, {:03.2f})", i, namePhysical, nameLogical,
                     nameMaterial, position.x(), position.y(), position.z());
    }
    spdlog::info("DetectorConstruction::PrintGeometryInfo - End");
}

bool DetectorConstruction::CheckOverlaps() const {
    spdlog::debug("DetectorConstruction::CheckOverlaps");
    return fWorld->CheckOverlaps();
}
