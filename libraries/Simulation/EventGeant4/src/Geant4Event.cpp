//
// Created by lobis on 23/11/2021.
//

#include "Geant4Event.h"

#include <spdlog/spdlog.h>

ClassImp(Geant4Event);

using namespace std;

namespace fmt {
template <>
struct formatter<TVector3> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.end(); }

    template <typename FormatContext>
    auto format(const TVector3& input, FormatContext& ctx) -> decltype(ctx.out()) {
        return format_to(ctx.out(), "({:0.2f}, {:0.2f}, {:0.2f})", input.x(), input.y(), input.z());
    }
};
}  // namespace fmt

void Geant4Event::Print() const {
    spdlog::info("EventID: {} - SubEventID {} - Total Sensitive Energy: {:0.2f} keV", fEventID, fSubEventID, fSensitiveVolumesTotalEnergy);
    spdlog::info("Primary: {} - Energy {:0.2f} keV - Position {} mm - Direction {}", fPrimaryParticleName, fPrimaryEnergy, fPrimaryPosition,
                 fPrimaryMomentum);
    if (fSubEventID != 0) {
        spdlog::info("SubEvent Primary: {} - Energy {:0.2f} keV - Position {} mm - Direction {}", fSubEventPrimaryParticleName,
                     fSubEventPrimaryEnergy, fSubEventPrimaryPosition, fSubEventPrimaryMomentum);
    }
    for (const auto& track : fTracks) {
        track.Print();
    }
}

void Geant4Event::PrintSensitiveInfo() const {
    spdlog::info("Geant4Event::PrintSensitiveInfo - EventID: {} - SubEventID: {}", fEventID, fSubEventID);
    spdlog::info("\tTotal energy in sensitive detectors: {:0.2f} keV", fSensitiveVolumesTotalEnergy);
    for (int i = 0; i < fSensitiveVolumeEnergy.size(); i++) {
        spdlog::info("\t - Sensitive Detector: {} (ID {}) E = {:0.2} keV", fSensitiveVolumeName[i], fSensitiveVolumeID[i], fSensitiveVolumeEnergy[i]);
    }
}

Int_t Geant4Event::GetTrackIndexByID(int trackID) {
    spdlog::debug("Geant4Event::GetTrackIndexByID - TrackID: {}", trackID);

    if (fTrackIDToTrackIndex.count(trackID) > 0) {
        const auto& track = fTracks[fTrackIDToTrackIndex.at(trackID)];
        if (track.fTrackID == trackID) {
            spdlog::debug("Geant4Event::GetTrackIndexByID - Found TrackID {} in store", trackID);
            return fTrackIDToTrackIndex.at(trackID);
        }
    }

    for (int i = 0; i < fTracks.size(); i++) {
        const auto& track = fTracks[i];
        if (track.fTrackID == trackID) {
            spdlog::debug("Geant4Event::GetTrackIndexByID - TrackID {} not found in store", trackID);
            fTrackIDToTrackIndex[trackID] = i;
            return i;
        }
    }

    spdlog::error(
        "Geant4Event::GetTrackIndexByID - Track ID {} not found in event. Maybe this is the parent of a track in a subEventID > 0? In this case it "
        "is stored in another event and cannot be accessed here",
        trackID);
    exit(1);
}

const Geant4Track& Geant4Event::GetTrackByID(int trackID) {
    // Split the method into 'GetTrackIndexByID' to fix an exception in PyROOT (which I don't understand)
    const auto& track = fTracks[GetTrackIndexByID(trackID)];
    return track;
}

bool Geant4Event::IsTrackSubEventPrimary(int trackID) {
    const auto& track = GetTrackByID(trackID);

    bool isSubEventPrimary = true;
    // check if track/s is/are the primary of sub-event
    if (fTrackIDToTrackIndex.count(track.fParentID) > 0) {
        // already cached
        isSubEventPrimary = false;
    } else {
        for (int i = 0; i < fTracks.size(); i++) {
            const auto& insertedTrack = fTracks[i];
            if (insertedTrack.fTrackID == track.fParentID) {
                // parent is registered
                isSubEventPrimary = false;
                fTrackIDToTrackIndex[insertedTrack.fTrackID] = i;
                break;
            }
        }
    }

    return isSubEventPrimary;
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
