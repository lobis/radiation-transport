//
// Created by lobis on 11/24/2021.
//

#include "DetectorConstruction.h"

#include <DetectorConstructionConfig.h>
#include <Geant4EventHeader.h>
#include <GlobalManager.h>
#include <SensitiveDetector.h>
#include <TString.h>
#include <spdlog/spdlog.h>

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
#include <map>
#include <set>
#include <string>
#include <vector>

#include "Exceptions.h"

using namespace std;

namespace fs = std::filesystem;

DetectorConstruction::DetectorConstruction() = default;

DetectorConstruction::DetectorConstruction(const DetectorConstructionConfig& detectorConfig)
    : fGeometryFilename(detectorConfig.GetGeometryAbsolutePath()) {
    spdlog::info("DetectorConstruction::DetectorConstruction - filename: '{}'", fGeometryFilename.c_str());
    if (string(fGeometryFilename).empty()) {
        spdlog::error("Geometry file not defined in DetectorConstruction");
        throw exceptions::NoGeometryFile;
    }
    if (!fs::exists(fGeometryFilename)) {
        spdlog::error("Geometry file '{}' not found", fGeometryFilename.c_str());
        throw exceptions::GeometryFileNotFound;
    }
}

DetectorConstruction::~DetectorConstruction() = default;

G4VPhysicalVolume* DetectorConstruction::Construct() {
    spdlog::debug("DetectorConstruction::Construct");
    spdlog::info("DetectorConstruction::Construct - Reading geometry from '{}'", fGeometryFilename.c_str());

    auto geometryInfo = new Geant4GeometryInfo();
    GlobalManager::Instance()->GetEventHeader()->fGeant4GeometryInfo = geometryInfo;
    GlobalManager::Instance()->GetEventHeader()->fSimulationConfig = &GlobalManager::Instance()->GetSimulationConfig();
    // read extension
    if (fGeometryFilename.extension() == ".gdml") {
        spdlog::info("Reading geometry as GDML");
        G4GDMLParser parser;
        auto geometryPath = fGeometryFilename.parent_path();
        auto originalPath = fs::current_path();
        if (originalPath != geometryPath) {
            spdlog::warn("Changing path to '{}' in order to read gdml", geometryPath.c_str());
            std::filesystem::current_path(geometryPath);
        }
        parser.Read(fGeometryFilename.c_str(), true);
        std::filesystem::current_path(originalPath);

        geometryInfo->PopulateFromGdml(fGeometryFilename.c_str());
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

    geometryInfo->PopulateFromGeant4World(fWorld);

    auto replacementMap = GlobalManager::Instance()->GetSimulationConfig().fDetectorConfig.fReplaceMaterialsMap;
    if (!replacementMap.empty()) {
        for (const auto& [logicalVolumeToReplaceMaterialName, newMaterialNameToReplace] : replacementMap) {
            bool volumeFound = false;
            for (const auto& logicalName : geometryInfo->GetAllLogicalVolumes()) {
                if (logicalName == logicalVolumeToReplaceMaterialName) {
                    auto logical = G4LogicalVolumeStore::GetInstance()->GetVolume(logicalName.Data());
                    auto materialNameOld = logical->GetMaterial()->GetName();
                    spdlog::warn("DetectorConstruction::Construct - Replacing logical volume '{}' material '{}' by '{}'.",
                                 logicalVolumeToReplaceMaterialName, materialNameOld, newMaterialNameToReplace);
                    auto materialNew = G4NistManager::Instance()->FindOrBuildMaterial(newMaterialNameToReplace, true, true);
                    if (!materialNew) {
                        spdlog::error("DetectorConstruction::Construct - New material '{}' to use as replacement for '{}' in '{}' not found.",
                                      newMaterialNameToReplace, materialNameOld, logicalVolumeToReplaceMaterialName);
                        exit(1);
                    }
                    logical->SetMaterial(materialNew);

                    volumeFound = true;
                    break;
                }
            }
            if (!volumeFound) {
                spdlog::error(
                    "DetectorConstruction::Construct - Could not replaced logical volume '{}' material by '{}'. Logical volume not found by name",
                    logicalVolumeToReplaceMaterialName, newMaterialNameToReplace);
                exit(1);
            }
        }
        // reset geometry info
        delete geometryInfo;
        geometryInfo = new Geant4GeometryInfo();
        if (fGeometryFilename.extension() == ".gdml") {
            geometryInfo->PopulateFromGdml(fGeometryFilename.c_str());
        }
        geometryInfo->PopulateFromGeant4World(fWorld);
    }

    return fWorld;
}

void DetectorConstruction::ConstructSDandField() {
    spdlog::debug("DetectorConstruction::ConstructSDandField");

    DetectorConstructionConfig config = GlobalManager::Instance()->GetSimulationConfig().fDetectorConfig;

    vector<string> sensitiveVolumes;  // user submitted sensitive volumes, may not exist or not be physical (be logical)
    map<string, bool> sensitiveVolumeIsKill = {};
    for (const auto& volume : config.fVolumes) {
        if (volume.fIsSensitive) {
            sensitiveVolumes.emplace_back(volume.fName);
            sensitiveVolumeIsKill[volume.fName] = volume.fKill;
        }
    }

    if (sensitiveVolumes.empty()) {
        return;
    }

    set<G4LogicalVolume*> logicalVolumesSelected;
    for (const auto& userSensitiveVolume : sensitiveVolumes) {
        spdlog::debug("DetectorConstruction::ConstructSDandField: User selected volume: {}", userSensitiveVolume);
        G4LogicalVolume* logicalVolume;
        G4VPhysicalVolume* physicalVolume = G4PhysicalVolumeStore::GetInstance()->GetVolume(userSensitiveVolume, false);
        if (!physicalVolume) {
            // perhaps user selected a logical volume with this name
            logicalVolume = G4LogicalVolumeStore::GetInstance()->GetVolume(userSensitiveVolume, false);
        } else {
            logicalVolume = physicalVolume->GetLogicalVolume();
        }
        if (!logicalVolume) {
            auto logicalVolumes =
                GlobalManager::Instance()->GetEventHeader()->GetGeant4GeometryInfo()->GetAllLogicalVolumesMatchingExpression(userSensitiveVolume);
            if (logicalVolumes.empty()) {
                PrintGeometryInfo();
                spdlog::error(
                    "Trying to attach a sensitive detector to user selected volume '{}'"
                    ", but this physical volume is not found in physical or logical store. Please read the geometry info printed above to find the "
                    "correct name",
                    userSensitiveVolume);
                exit(1);
            } else {
                for (const auto& logicalVolumeName : logicalVolumes) {
                    spdlog::info("DetectorConstruction::ConstructSDandField: Logical volume from regex '{}'", logicalVolumeName);
                    auto logicalVolume = G4LogicalVolumeStore::GetInstance()->GetVolume(logicalVolumeName.Data(), false);
                    logicalVolumesSelected.insert(logicalVolume);
                    sensitiveVolumeIsKill[logicalVolume->GetName()] = sensitiveVolumeIsKill[userSensitiveVolume];
                }
                continue;
            }
        }
        logicalVolumesSelected.insert(logicalVolume);
        sensitiveVolumeIsKill[logicalVolume->GetName()] = sensitiveVolumeIsKill[userSensitiveVolume];
        if (userSensitiveVolume != logicalVolume->GetName()) {
            sensitiveVolumeIsKill.erase(userSensitiveVolume);
        }
    }

    /*
    spdlog::warn("DetectorConstruction::ConstructSDandField - debug kill");
    for (const auto& [volumeName, killFlag] : sensitiveVolumeIsKill) {
        spdlog::warn("{} - {}", volumeName, killFlag ? "True" : "False");
    }
    */
    G4SDManager* SDManager = G4SDManager::GetSDMpointer();

    for (G4LogicalVolume* logicalVolume : logicalVolumesSelected) {
        auto name = logicalVolume->GetName();
        spdlog::info("DetectorConstruction::ConstructSDandField: Attaching sensitive detector to logical volume '{}'", name);
        bool kill = sensitiveVolumeIsKill.at(name);
        G4VSensitiveDetector* sensitiveDetector = new SensitiveDetector(name, kill);
        SDManager->AddNewDetector(sensitiveDetector);
        logicalVolume->SetSensitiveDetector(sensitiveDetector);

        auto region = new G4Region(name);
        logicalVolume->SetRegion(region);
    }
}

std::vector<G4LogicalVolume*> DetectorConstruction::GetAllLogicalVolumes() const {
    auto logicalVolumes = std::vector<G4LogicalVolume*>();

    for (int i = 0; i < fWorld->GetLogicalVolume()->GetNoDaughters(); i++) {
        auto logical = fWorld->GetLogicalVolume()->GetDaughter(i)->GetLogicalVolume();

        bool inserted = false;
        for (const auto& insertedLogical : logicalVolumes) {
            if (insertedLogical == logical) {
                inserted = true;
                break;
            }
        }
        if (inserted) {
            continue;
        }

        logicalVolumes.push_back(logical);
    }

    return logicalVolumes;
}

void DetectorConstruction::PrintGeometryInfo() {
    spdlog::info("DetectorConstruction::PrintGeometryInfo - Begin");
    const int n = int(fWorld->GetLogicalVolume()->GetNoDaughters());
    for (int i = 0; i < n + 1; i++) {
        G4VPhysicalVolume* volume;
        if (i == n) {
            volume = fWorld;
        } else {
            volume = fWorld->GetLogicalVolume()->GetDaughter(i);
        }
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

void Geant4GeometryInfo::PopulateFromGeant4World(const G4VPhysicalVolume* world) {
    const int n = int(world->GetLogicalVolume()->GetNoDaughters());
    for (int i = 0; i < n + 1; i++) {  // world is the + 1
        G4VPhysicalVolume* volume;
        if (i == n) {
            volume = const_cast<G4VPhysicalVolume*>(world);
        } else {
            volume = world->GetLogicalVolume()->GetDaughter(i);
        }
        TString namePhysical = (TString)volume->GetName();
        if (fGdmlNewPhysicalNames.size() > i) {
            // it has been filled
            fGeant4PhysicalNameToNewPhysicalNameMap[namePhysical] = fGdmlNewPhysicalNames[i];
        }
        TString physicalNewName = GetAlternativeNameFromGeant4PhysicalName(namePhysical);
        TString nameLogical = (TString)volume->GetLogicalVolume()->GetName();
        TString nameMaterial = (TString)volume->GetLogicalVolume()->GetMaterial()->GetName();
        auto position = volume->GetTranslation();

        spdlog::info(
            "Geant4GeometryInfo::PopulateFromGeant4World - {} - physical: {} ({}) - logical: {} - material: {} - position: ({:0.2f}, {:0.2f}, "
            "{:0.2f})",
            i, namePhysical.Data(), physicalNewName.Data(), nameLogical.Data(), nameMaterial.Data(), position.x(), position.y(), position.z());

        fPhysicalToLogicalVolumeMap[namePhysical] = nameLogical;
        fLogicalToMaterialMap[nameLogical] = nameMaterial;
        fLogicalToPhysicalMap[nameLogical].emplace_back(namePhysical);
        fPhysicalToPositionInWorldMap[namePhysical] = {position.x(), position.y(), position.z()};
    }
}
