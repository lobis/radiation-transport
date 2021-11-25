//
// Created by lobis on 23/04/2021.
//

#include "TrackingAction.h"

#include <G4EventManager.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTypes.hh>
#include <G4RunManager.hh>
#include <G4SteppingManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4Track.hh>
#include <G4UnitsTable.hh>

#include "EventAction.h"
#include "GlobalManager.h"
#include "OutputManager.h"
#include "SteppingAction.h"
#include "spdlog/spdlog.h"

TrackingAction::TrackingAction() : G4UserTrackingAction() {
    spdlog::debug("TrackingAction::TrackingAction");
    output = OutputManager::Instance();
}

void TrackingAction::PreUserTrackingAction(const G4Track* track) { output->RecordTrack(track); }

void TrackingAction::PostUserTrackingAction(const G4Track* track) {
    auto particle = track->GetParticleDefinition();

    auto steppingAction = (SteppingAction*)G4EventManager::GetEventManager()->GetUserSteppingAction();
    auto fSecondary = steppingAction->GetfSecondary();

    spdlog::debug("TrackingAction::PostUserTrackingAction ---> TrackID: {:4d}, ParentID: {:4d}, Particle: {:>10}, Secondaries: {:2d}",
                  track->GetTrackID(), track->GetParentID(), particle->GetParticleName(), fSecondary->size());

    for (const auto& secondary : *fSecondary) {
        G4String energyWithUnits = G4BestUnit(secondary->GetKineticEnergy(), "Energy");
        auto creatorProcess = secondary->GetCreatorProcess();
        spdlog::debug("TrackingAction::PostUserTrackingAction ---> ---> Secondary: {:>10}, KE:  {:>15}, Creator Process: {:>20} ({})",
                      secondary->GetDynamicParticle()->GetDefinition()->GetParticleName(), energyWithUnits, creatorProcess->GetProcessName(),
                      G4VProcess::GetProcessTypeName(creatorProcess->GetProcessType()));
    }

    output->UpdateTrack(track);
}
