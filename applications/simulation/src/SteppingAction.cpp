//
// Created by lobis on 5/2/2021.
//
#include "SteppingAction.h"

#include <spdlog/spdlog.h>

#include <G4OpticalPhoton.hh>
#include <G4RunManager.hh>
#include <G4Step.hh>
#include <G4StepPoint.hh>
#include <G4SteppingManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4Track.hh>

#include "DetectorConstruction.h"
#include "EventAction.h"
#include "OutputManager.h"

SteppingAction::SteppingAction() { output = OutputManager::Instance(); }

SteppingAction::~SteppingAction() = default;

void SteppingAction::UserSteppingAction(const G4Step* step) {
    auto track = step->GetTrack();

    output->RecordStep(step);

    if (track->GetTrackStatus() == fAlive) {
        return;
    }
}

G4TrackVector* SteppingAction::GetfSecondary() {
    G4SteppingManager* steppingManager = fpSteppingManager;
    return steppingManager->GetfSecondary();
}
