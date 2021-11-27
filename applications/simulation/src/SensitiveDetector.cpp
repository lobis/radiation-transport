//
// Created by lobis on 29/06/2021.
//

#include "SensitiveDetector.h"

#include <G4Step.hh>
#include <G4SystemOfUnits.hh>
#include <G4Track.hh>
#include <G4VProcess.hh>

#include "OutputManager.h"
#include "spdlog/spdlog.h"

using namespace std;

SensitiveDetector::SensitiveDetector(const G4String& name) : G4VSensitiveDetector(name) {
    output = OutputManager::Instance();
    spdlog::info("SensitiveDetector::SensitiveDetector '{}'", name);
}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*) {
    spdlog::debug("SensitiveDetector::ProcessHits");

    /*
    const auto track = step->GetTrack();
    const auto particle = track->GetParticleDefinition();
    const auto& particleName = particle->GetParticleName();

    if (particleName == "opticalphoton") {
        track->SetTrackStatus(fStopAndKill);
        output->AddOpticalPhoton(track->GetCreatorProcess()->GetProcessName());
    }
    */

    auto energy = step->GetTotalEnergyDeposit() / keV;

    if (energy <= 0) {
        return true;
    }

    const auto volumeName = step->GetPostStepPoint()->GetPhysicalVolume()->GetName();

    output->AddSensitiveEnergy(energy, volumeName);

    return true;  // return value will always be ignored
}