//
// Created by lobis on 23/11/2021.
//

#include "Geant4Event.h"

#include <spdlog/spdlog.h>

#include <G4Event.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4Track.hh>
#include <G4VUserPrimaryVertexInformation.hh>

#include "OutputManager.h"

using namespace std;

Geant4Event::Geant4Event(const G4Event* event) : Geant4Event() {
    fRunID = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();
    fEventID = event->GetEventID();
    fSubEventID = 0;

    // this only words when there is a single particle primary vertex!
    auto primaryVertex = event->GetPrimaryVertex();
    if (primaryVertex->GetNumberOfParticle() > 1) {
        spdlog::warn("multiple primary particles detected, but only recording first particle!");
    }
    const auto& position = primaryVertex->GetPosition();
    fPrimaryPosition = TVector3(position.x() / CLHEP::mm, position.y() / CLHEP::mm, position.z() / CLHEP::mm);
    auto primaryParticle = primaryVertex->GetPrimary();
    fPrimaryParticleName = primaryParticle->GetParticleDefinition()->GetParticleName();
    fPrimaryEnergy = primaryParticle->GetKineticEnergy() / CLHEP::keV;
    const auto& momentum = primaryParticle->GetMomentumDirection();
    fPrimaryMomentum = TVector3(momentum.x() / CLHEP::mm, momentum.y() / CLHEP::mm, momentum.z() / CLHEP::mm);
}

bool Geant4Event::InsertTrack(const G4Track* track) {
    if (!OutputManager::Instance()->IsValidTrack(track)) {
        return false;
    }
    if (fInitialStep.fN != 1) {
        spdlog::error("fInitialStep does not have exactly one step! Problem with stepping verbose");
        exit(1);
    }
    fTracks.emplace_back(track);
    fTracks.back().fHits = fInitialStep;

    fTrackIDToTrackIndex[track->GetTrackID()] = fTracks.size() - 1;

    if (fTracks.empty()) {
        // primary for the sub-event
        fSubEventPrimaryParticleName = track->GetParticleDefinition()->GetParticleName();
        fSubEventPrimaryEnergy = track->GetKineticEnergy() / CLHEP::keV;
        const auto& position = track->GetPosition();
        fSubEventPrimaryPosition = TVector3(position.x() / CLHEP::mm, position.y() / CLHEP::mm, position.z() / CLHEP::mm);
        const auto& momentum = track->GetMomentumDirection();
        fSubEventPrimaryMomentum = TVector3(momentum.x() / CLHEP::mm, momentum.y() / CLHEP::mm, momentum.z() / CLHEP::mm);
    }
    return true;
}

void Geant4Event::UpdateTrack(const G4Track* track) {
    if (!OutputManager::Instance()->IsValidTrack(track)) {
        return;
    }
    fTracks.back().UpdateTrack(track);
}

void Geant4Event::InsertStep(const G4Step* step) {
    if (!OutputManager::Instance()->IsValidStep(step)) {
        return;
    }
    if (step->GetTrack()->GetCurrentStepNumber() == 0) {
        // initial step (from SteppingVerbose) is generated before TrackingAction can insert the first track
        fInitialStep = Geant4Hits();
        fInitialStep.InsertStep(step);
    } else {
        fTracks.back().InsertStep(step);
    }
}
