//
// Created by lobis on 29/06/2021.
//

#include "SensitiveDetector.h"

#include <spdlog/spdlog.h>

#include <G4Step.hh>
#include <G4SystemOfUnits.hh>
#include <G4Track.hh>
#include <G4VProcess.hh>

using namespace std;

SensitiveDetector::SensitiveDetector(const G4String& name, bool kill) : G4VSensitiveDetector(name), fOutput(OutputManager::Instance()), fKill(kill) {
    spdlog::info("SensitiveDetector::SensitiveDetector '{}'{}", name, fKill ? " (kill)" : "");
}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*) {
    spdlog::debug("SensitiveDetector::ProcessHits");

    if (fKill) {
        /*
        spdlog::warn("KILLING TRACK IN {} {} {} {}", step->GetPreStepPoint()->GetPhysicalVolume()->GetName(),
                     step->GetPostStepPoint()->GetPhysicalVolume()->GetName(), step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName(),
                     step->GetTrack()->GetTrackID());
        */
        fOutput->SetForceSave(true);
        step->GetTrack()->SetTrackStatus(fStopAndKill);

        return true;
    }

    auto energy = step->GetTotalEnergyDeposit() / keV;

    if (energy <= 0) {
        return true;
    }

    const auto volumeName = (TString)step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
    // TODO: Check this, may not work as expected in MT mode
    fOutput->AddSensitiveEnergy(energy, volumeName);

    return true;  // return value will always be ignored
}