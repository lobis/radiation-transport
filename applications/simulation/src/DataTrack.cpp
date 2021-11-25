//
// Created by lobis on 23/11/2021.
//

#include "DataTrack.h"

#include <spdlog/spdlog.h>

#include <G4ParticleDefinition.hh>
#include <G4Step.hh>
#include <G4Track.hh>
#include <G4UnitsTable.hh>
#include <G4VProcess.hh>

using namespace std;

DataTrack::DataTrack(const G4Track* track) {
    fTrackID = track->GetTrackID();
    fParentID = track->GetParentID();

    auto particle = track->GetParticleDefinition();
    fParticleName = particle->GetParticleName();
    fParticleID = particle->GetPDGEncoding();
    fParticleType = particle->GetParticleType();
    fParticleSubType = particle->GetParticleSubType();

    auto creatorProcess = track->GetCreatorProcess();
    if (creatorProcess) {
        fCreatorProcess = creatorProcess->GetProcessName();
    }

    fInitialKineticEnergy = track->GetKineticEnergy() / CLHEP::keV;

    fWeight = track->GetWeight();

    //
    G4String energyWithUnits = G4BestUnit(fInitialKineticEnergy * CLHEP::keV, "Energy");
    spdlog::debug("DataTrack::DataTrack - Track ID {} - Parent ID {} - Particle {} ({}) created by {} - Energy {}", fTrackID, fParentID,
                  fParticleName, fParticleID, (fCreatorProcess.IsNull() ? "IS-PRIMARY-PARTICLE" : fCreatorProcess), energyWithUnits);
}

void DataTrack::InsertStep(const G4Step* step) { fSteps.InsertStep(step); }

void DataTrack::UpdateTrack(const G4Track* track) {
    if (track->GetTrackID() != fTrackID) {
        spdlog::error("DataTrack::UpdateTrack - mismatch of trackID!");
        exit(1);
    }

    fTrackLength = track->GetTrackLength() / CLHEP::mm;

    // auto steppingAction = (SteppingAction*)G4EventManager::GetEventManager()->GetUserSteppingAction();
    // auto secondaries = steppingAction->GetfSecondary();
    // fNumberOfSecondaries = (int)secondaries->size();
}
