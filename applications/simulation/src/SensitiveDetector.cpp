//
// Created by lobis on 29/06/2021.
//

#include "SensitiveDetector.h"

#include <G4Step.hh>
#include <G4SystemOfUnits.hh>
#include <G4Track.hh>
#include <G4VProcess.hh>

#include "spdlog/spdlog.h"

using namespace std;

SensitiveDetector::SensitiveDetector(const G4String& name) : G4VSensitiveDetector(name), fOutput(OutputManager::Instance()) {
    spdlog::info("SensitiveDetector::SensitiveDetector '{}'", name);
}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*) {
    spdlog::debug("SensitiveDetector::ProcessHits");

    auto energy = step->GetTotalEnergyDeposit() / keV;

    if (energy <= 0) {
        return true;
    }

    const G4String volumeName = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
    // TODO: Check this, may not work as expected in MT mode
    fOutput->AddSensitiveEnergy(energy, volumeName);

    return true;  // return value will always be ignored
}