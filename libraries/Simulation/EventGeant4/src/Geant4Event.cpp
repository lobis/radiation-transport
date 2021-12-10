//
// Created by lobis on 23/11/2021.
//

#include "Geant4Event.h"

#include <spdlog/spdlog.h>

ClassImp(Geant4Event);

using namespace std;

void Geant4Event::Print() const { spdlog::info("Not implemented yet!"); }

void Geant4Event::PrintSensitiveInfo() const {
    spdlog::info("Geant4Event::PrintSensitiveInfo - EventID: {} - SubEventID: {}", fEventID, fSubEventID);
    spdlog::info("\tTotal energy in sensitive detectors: {:0.2f} keV", fSensitiveVolumesTotalEnergy);
    for (int i = 0; i < fSensitiveVolumeEnergy.size(); i++) {
        spdlog::info("\t - Sensitive Detector: {} (ID {}) E = {:0.2} keV", fSensitiveVolumeName[i], fSensitiveVolumeID[i], fSensitiveVolumeEnergy[i]);
    }
}

const Geant4Track& Geant4Event::GetTrackByID(int trackID) {
    spdlog::debug("Geant4Event::GetTrackByID - TrackID: {}", trackID);

    if (fTrackIDToTrackIndex.count(trackID) > 0) {
        const auto& track = fTracks[fTrackIDToTrackIndex.at(trackID)];
        if (track.fTrackID == trackID) {
            spdlog::debug("Geant4Event::GetTrackByID - Found TrackID {} in store", trackID);
            return track;
        }
    }

    for (int i = 0; i < fTracks.size(); i++) {
        const auto& track = fTracks[fTrackIDToTrackIndex.at(trackID)];
        if (track.fTrackID == trackID) {
            spdlog::debug("Geant4Event::GetTrackByID - TrackID {} not found in store", trackID);
            fTrackIDToTrackIndex[trackID] = i;
            return track;
        }
    }
    spdlog::error("Geant4Event::GetTrackByID - Track ID {} not found in event", trackID);
    exit(1);
}

double Geant4Event::GetEnergyInVolume(const TString& volume) {
    if (fEnergyInVolumeMap.empty()) {
        InitializeEnergyInVolumeMap();
    }
    if (fEnergyInVolumeMap.count(volume.Data()) > 0) {
        return fEnergyInVolumeMap.at(volume.Data());
    }
    return 0;
}

void Geant4Event::InitializeEnergyInVolumeMap() {
    fEnergyInVolumeMap.clear();
    auto volumes = fEventHeader->fGeant4GeometryInfo->GetAllPhysicalVolumes();
    for (auto& track : fTracks) {
        for (const auto& volume : volumes) {
            fEnergyInVolumeMap[volume.Data()] = fEnergyInVolumeMap[volume.Data()] + track.GetEnergyInVolume(volume);
        }
    }
}
