//
// Created by lobis on 23/11/2021.
//

#include "Geant4Track.h"

#include <spdlog/spdlog.h>

#include <G4ParticleDefinition.hh>
#include <G4Step.hh>
#include <G4Track.hh>
#include <G4UnitsTable.hh>
#include <G4VProcess.hh>

using namespace std;

Geant4Track::Geant4Track(const G4Track* track) : Geant4Track() {
    fTrackID = track->GetTrackID();
    fParentID = track->GetParentID();

    auto particle = track->GetParticleDefinition();
    fParticleName = particle->GetParticleName();
    fParticleID = particle->GetPDGEncoding();
    fParticleType = particle->GetParticleType();
    fParticleSubType = particle->GetParticleSubType();

    if (track->GetCreatorProcess()) {
        fCreatorProcess = track->GetCreatorProcess()->GetProcessName();
    } else {
        fCreatorProcess = "IsPrimaryParticle";
    }

    fInitialKineticEnergy = track->GetKineticEnergy() / CLHEP::keV;

    fWeight = track->GetWeight();

    G4String energyWithUnits = G4BestUnit(fInitialKineticEnergy * CLHEP::keV, "Energy");
    spdlog::debug("Geant4Track::Geant4Track - Track ID {} - Parent ID {} - Particle {} ({}) created by {} - Energy {}", fTrackID, fParentID,
                  fParticleName, fParticleID, fCreatorProcess, energyWithUnits);
}

void Geant4Track::InsertStep(const G4Step* step) { fHits.InsertStep(step); }

void Geant4Track::UpdateTrack(const G4Track* track) {
    if (track->GetTrackID() != fTrackID) {
        spdlog::error("Geant4Track::UpdateTrack - mismatch of trackID!");
        exit(1);
    }

    fTrackLength = track->GetTrackLength() / CLHEP::mm;

    // auto steppingAction = (SteppingAction*)G4EventManager::GetEventManager()->GetUserSteppingAction();
    // auto secondaries = steppingAction->GetfSecondary();
    // fNumberOfSecondaries = (int)secondaries->size();
}
