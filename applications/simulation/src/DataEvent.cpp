//
// Created by lobis on 23/11/2021.
//

#include "DataEvent.h"

#include <spdlog/spdlog.h>

#include <G4Event.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4Track.hh>
#include <G4VUserPrimaryVertexInformation.hh>

using namespace std;

DataEvent::DataEvent(const G4Event* event) : DataEvent() {
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

void DataEvent::InsertTrack(const G4Track* track) {
    if (!IsValid(track)) {
        return;
    }

    if (fTracks.empty()) {
        // primary for the subevent
        fSubEventPrimaryParticleName = track->GetParticleDefinition()->GetParticleName();
        fSubEventPrimaryEnergy = track->GetKineticEnergy() / CLHEP::keV;
        const auto& position = track->GetPosition();
        fSubEventPrimaryPosition = TVector3(position.x() / CLHEP::mm, position.y() / CLHEP::mm, position.z() / CLHEP::mm);
        const auto& momentum = track->GetMomentumDirection();
        fSubEventPrimaryMomentum = TVector3(momentum.x() / CLHEP::mm, momentum.y() / CLHEP::mm, momentum.z() / CLHEP::mm);
    }
    fTracks.emplace_back(track);
    if (fInitialStep.fN != 1) {
        spdlog::error("fInitialStep does not have exactly one step! Problem with stepping verbose");
        exit(1);
    }
    fTracks.back().fSteps = fInitialStep;
}

void DataEvent::UpdateTrack(const G4Track* track) {
    if (!IsValid(track)) {
        return;
    }
    fTracks.back().UpdateTrack(track);
}

void DataEvent::InsertStep(const G4Step* step) {
    if (!IsValid(step)) {
        return;
    }
    if (step->GetTrack()->GetCurrentStepNumber() == 0) {
        // initial step (from SteppingVerbose) is generated before TrackingAction can insert the first track
        fInitialStep = DataSteps();
        fInitialStep.InsertStep(step);
    } else {
        fTracks.back().InsertStep(step);
    }
}

bool DataEvent::IsValid(const G4Track* track) {
    // return true;
    // optical photons take too much space to store them
    for (const auto& particleName : {"opticalphoton"}) {
        if (track->GetParticleDefinition()->GetParticleName() == particleName) {
            return false;
        }
    }
    return true;
}

bool DataEvent::IsValid(const G4Step* step) { return IsValid(step->GetTrack()); }
