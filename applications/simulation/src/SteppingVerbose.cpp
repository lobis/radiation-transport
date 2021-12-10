//
// Created by lobis on 17/09/2021.
//

#include "SteppingVerbose.h"

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

SteppingVerbose::SteppingVerbose() : G4SteppingVerbose() { output = OutputManager::Instance(); }
SteppingVerbose::~SteppingVerbose() {}

void SteppingVerbose::TrackingStarted() {
    CopyState();
    output->RecordStep(fStep);
}

void SteppingVerbose::StepInfo() {}
