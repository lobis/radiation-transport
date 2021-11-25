//
// Created by lobis on 29/06/2021.
//

#include "RunAction.h"

#include <TGeoManager.h>
#include <TString.h>
#include <unistd.h>

#include <G4IonTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTable.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4SystemOfUnits.hh>

#include "DetectorConstruction.h"
#include "GlobalManager.h"
#include "OutputManager.h"
#include "SteppingVerbose.h"
#include "spdlog/spdlog.h"

using namespace std;

RunAction::RunAction() {
    if (G4Threading::IsMasterThread() && G4Threading::IsMultithreadedApplication()) {
        output = nullptr;  // no need for output, helps prevent bugs
    } else {
        output = OutputManager::Instance();
    }
}

RunAction::~RunAction() = default;

void RunAction::BeginOfRunAction(const G4Run* aRun) {
    spdlog::info("RunAction::BeginOfRunAction ---> Begin of run {}", aRun->GetRunID());

    if (G4Threading::IsMasterThread()) {
        GlobalManager::Instance()->SetupFile();
    }

    auto steppingVerbose = ((SteppingVerbose*)G4VSteppingVerbose::GetInstance());
    steppingVerbose->SetSteppingVerbose(1);
}

void RunAction::EndOfRunAction(const G4Run* aRun) {
    spdlog::info("RunAction::EndOfRunAction <--- End of run {} (thread {})", aRun->GetRunID(), G4Threading::G4GetThreadId());

    if (G4Threading::IsMasterThread() && !GlobalManager::Instance()->GetSimulationConfig().fInteractive) {
        spdlog::info("RunAction::EndOfRunAction <--- Writing events to file");
        GlobalManager::Instance()->WriteEventsAndCloseFile();
    }
}