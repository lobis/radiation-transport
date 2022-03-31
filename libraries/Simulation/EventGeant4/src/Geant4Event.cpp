//
// Created by lobis on 23/11/2021.
//

#include "Geant4Event.h"

#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>

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

/*
Int_t Geant4Event::GetTrackIndexByID(int trackID) {
    spdlog::info("Geant4Event::GetTrackIndexByID - TrackID: {}", trackID);

    if (fTrackIDToTrackIndex.count(trackID) > 0) {
        Int_t index = fTrackIDToTrackIndex.at(trackID);
        const auto& track = fTracks[index];
        if (track.fTrackID == trackID) {
            spdlog::info("Geant4Event::GetTrackIndexByID - Found TrackID {} in store at index {}", trackID, index);
            return index;
        }
    }

    for (int i = 0; i < fTracks.size(); i++) {
        const auto& track = fTracks[i];
        if (track.fTrackID == trackID) {
            spdlog::info("Geant4Event::GetTrackIndexByID - TrackID {} not found in store", trackID);
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
*/

const Geant4Track& Geant4Event::GetTrackByID(int trackID) const {
    for (int i = 0; i < fTracks.size(); i++) {
        const auto& track = fTracks[i];
        if (track.fTrackID == trackID) {
            return track;
        }
    }
    spdlog::error("Geant4Event::GetTrackByID - Error");
    exit(1);
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

double Geant4Event::GetEnergyInVolume(const TString& volume, const TString& processName) const {
    double energy = 0;
    for (const auto& track : fTracks) {
        energy += track.GetEnergyInVolume(volume, processName);
    }
    return energy;
}

map<TString, double> Geant4Event::GetSensitiveEnergyInVolumes() const {
    map<TString, double> energyMap;
    for (int i = 0; i < fSensitiveVolumeEnergy.size(); i++) {
        energyMap[fSensitiveVolumeName[i]] = fSensitiveVolumeEnergy[i];
    }

    return energyMap;
}

using json = nlohmann::json;

void to_json(json& j, const TVector3& v) { j = json{{"x", v.X()}, {"y", v.Y()}, {"z", v.Z()}}; }

void to_json(json& j, const Geant4Hits& hits) {
    j = json{
        {"stepID", hits.fStepID},     {"time", hits.fTimeGlobal},         {"energy", hits.fEnergy}, {"kineticEnergy", hits.fKineticEnergy},
        {"volumeName", hits.fStepID}, {"processName", hits.fProcessName},
    };
    // split position
    const size_t n = hits.fPosition.size();
    vector<Double_t> x(n), y(n), z(n);
    for (int i = 0; i < n; i++) {
        x[i] = hits.fPosition[i].X();
        y[i] = hits.fPosition[i].Y();
        z[i] = hits.fPosition[i].Z();
    }
    j["x"] = x;
    j["y"] = y;
    j["z"] = z;
}

void to_json(json& j, const Geant4Track& track) { j = json{{"trackID", track.fTrackID}, {"parentID", track.fParentID}, {"hits", track.fHits}}; }

void to_json(json& j, const Geant4Event& event) { j = json{{"eventID", event.fEventID}, {"tracks", event.fTracks}}; }

string Geant4Event::ToJson() const { return json{*this}.dump(); }
