//
// Created by lobis on 25/11/2021.
//

#include <Geant4GeometryInfo.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4VPhysicalVolume.hh>
#include <filesystem>

#include "DetectorConstruction.h"
#include "SimulationConfig.h"

#define EXAMPLES_PATH string(std::filesystem::path(__FILE__).parent_path().parent_path().parent_path()) + "/examples/"

using namespace std;

TEST(DetectorConstruction, LoadGdmlGeometry) {
    SimulationConfig config(EXAMPLES_PATH + "basic/simulation.yaml");

    DetectorConstruction detectorConstruction(config.fDetectorConfig);

    EXPECT_FALSE(detectorConstruction.GetWorld());

    auto world = detectorConstruction.Construct();

    EXPECT_EQ(detectorConstruction.GetWorld(), world);
}

TEST(DetectorConstruction, LoadFromConfig) {
    SimulationConfig config(EXAMPLES_PATH + "basic/simulation.yaml");

    DetectorConstruction detectorConstruction(config.fDetectorConfig);
}

TEST(DetectorConstruction, CheckGdmlGeometry) {
    SimulationConfig config(EXAMPLES_PATH + "basic/simulation.yaml");

    DetectorConstruction detectorConstruction(config.fDetectorConfig);

    auto world = detectorConstruction.Construct();

    EXPECT_EQ(world->GetLogicalVolume()->GetNoDaughters(), 1);

    const auto volume = world->GetLogicalVolume()->GetDaughter(0);

    const auto& namePhysical = volume->GetName();
    EXPECT_EQ(namePhysical, "box");
    const auto& nameLogical = volume->GetLogicalVolume()->GetName();
    EXPECT_EQ(nameLogical, "boxVolume");
    const auto& nameMaterial = volume->GetLogicalVolume()->GetMaterial()->GetName();
    EXPECT_EQ(nameMaterial, "G4_WATER");
    const auto& position = volume->GetTranslation();
    EXPECT_EQ(position, CLHEP::Hep3Vector(0, 0, 0));
}

TEST(DetectorConstruction, Geant4GeometryInfo) {
    SimulationConfig config(EXAMPLES_PATH + "iaxo/neutrons.simulation.yaml");

    DetectorConstruction detectorConstruction(config.fDetectorConfig);

    auto world = detectorConstruction.Construct();

    Geant4GeometryInfo geometryInfo;

    geometryInfo.PopulateFromGeant4World(world);

    geometryInfo.Print();

    auto expression = "^scintillatorVolume";

    auto logicalVolumes = geometryInfo.GetAllLogicalVolumesMatchingExpression(expression);
    auto physicalVolumes = std::vector<TString>();
    for (const auto& logical : logicalVolumes) {
        for (const auto& physical : geometryInfo.GetAllPhysicalVolumesFromLogical(logical)) {
            physicalVolumes.push_back(physical);
        }
    }
    spdlog::info("number of physical volumes matching logical volume expression '{}' is {}", expression, physicalVolumes.size());
    for (const auto& physical : physicalVolumes) {
        spdlog::info("\t- {}", physical);
    }

    EXPECT_EQ(physicalVolumes.size(), 72);  // number of vetoes
}