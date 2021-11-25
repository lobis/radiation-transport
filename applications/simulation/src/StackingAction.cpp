//
// Created by Luis on 4/30/2021.
//

#include "StackingAction.h"

#include <G4ParticleTable.hh>
#include <G4ParticleTypes.hh>
#include <G4SystemOfUnits.hh>
#include <G4Track.hh>
#include <G4UnitsTable.hh>
#include <G4VProcess.hh>

#include "OutputManager.h"
#include "spdlog/spdlog.h"

StackingAction::StackingAction() : G4UserStackingAction(), fMaxAllowedLifetime(100 / nanosecond) {
    output = OutputManager::Instance();

    fMaxAllowedLifetimeWithUnit = G4BestUnit(fMaxAllowedLifetime, "Time");
    spdlog::debug("StackingAction::StackingAction - 'fMaxAllowedLifetime' is {}", fMaxAllowedLifetimeWithUnit);

    fParticlesToIgnore = {
        G4NeutrinoE::Definition(),      G4AntiNeutrinoE::Definition(), G4NeutrinoMu::Definition(),
        G4AntiNeutrinoMu::Definition(), G4NeutrinoTau::Definition(),   G4AntiNeutrinoTau::Definition(),
    };
    for (const auto& particle : fParticlesToIgnore) {
        spdlog::debug("StackingAction::StackingAction ---> Particle to ignore: {}", particle->GetParticleName());
    }
}

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* track) {
    if (track->GetParentID() <= 0) {
        // always process the first track regardless
        return fUrgent;
    }

    auto particle = track->GetParticleDefinition();
    if (fParticlesToIgnore.find(particle) != fParticlesToIgnore.end()) {
        // ignore this track
        spdlog::debug("StackingAction::ClassifyNewTrack ---> Killing track with '{}' (is in 'fParticlesToIgnore')", particle->GetParticleName());
        return fKill;
    }

    if (particle == G4OpticalPhoton::Definition()) {
        // TODO: Add to a NEW optical stack! (how to add additional stacks?)
        // return fKill;
    }

    if (track->GetCreatorProcess()->GetProcessType() != G4ProcessType::fDecay) {
        return fUrgent;
    }

    if (particle->GetParticleType() == "nucleus" && !particle->GetPDGStable()) {
        // unstable nucleus
        if (particle->GetPDGLifeTime() > fMaxAllowedLifetime) {
            G4String energy = G4BestUnit(track->GetKineticEnergy(), "Energy");
            G4String lifeTime = G4BestUnit(particle->GetPDGLifeTime(), "Time");

            spdlog::debug("StackingAction::ClassifyNewTrack ---> Splitting unstable '{}' track (KE: {}, lifetime of {} > {})",
                          particle->GetParticleName(), energy, lifeTime, fMaxAllowedLifetimeWithUnit);
            /* Ignore for the time being...
            if (track->GetKineticEnergy() > 0) {
                spdlog::warn("StackingAction::ClassifyNewTrack ---> Killing unstable '{}' track (KE: {}). It's energy is not zero.",
                             particle->GetParticleName(), energy);
            }
            */
            //  TODO: kill/wait it when it stops, not before
            // return fKill;
            return fWaiting;
        }
    }
    return fUrgent;
}

void StackingAction::NewStage() {
    /*
     * Called after processing all Urgent tracks
     * Close event and start a new sub event if there are waiting tracks
     */
    spdlog::debug("StackingAction::NewStage");

    const Int_t subEventID = output->GetSubEventID();
    output->FinishAndSubmitEvent();
    output->SetSubEventID(subEventID + 1);
}

StackingAction::~StackingAction() = default;
