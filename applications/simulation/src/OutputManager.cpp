
#include "OutputManager.h"

#include <Geant4Event.h>
#include <spdlog/spdlog.h>

#include <G4RunManager.hh>
#include <G4Threading.hh>
#include <G4Track.hh>
#include <G4UnitsTable.hh>
#include <G4UserEventAction.hh>

#include "GlobalManager.h"

using namespace std;

thread_local OutputManager* OutputManager::pinstance_ = nullptr;

/*!
 * Returns the thread-local instance of the OutputManager
 * Each worker thread will use its own instance to save data
 * Master thread should never instance OutputManager unless its in Serial Mode
 * @return OutputManager*
 */
OutputManager* OutputManager::Instance() {
    if (G4Threading::IsMasterThread() && G4Threading::IsMultithreadedApplication()) {
        spdlog::warn("OutputManager::Instance() - Thread local instance should never be invoked from master thread in a MT application");
    }
    if (pinstance_ == nullptr) {
        pinstance_ = new OutputManager();
    }
    return pinstance_;
}

/*!
 * Updates the 'fEvent' with a new event. Since its a unique ptr, there is no need to delete the old event.
 * This should be called at the begin of event action
 * If the event is to be saved, it should have been already moved into the container via `OutputManager::FinishAndSubmitEvent` at the end of event
 * action.
 */
void OutputManager::UpdateEvent() {
    auto event = G4EventManager::GetEventManager()->GetConstCurrentEvent();
    fEvent = make_unique<Geant4Event>(event);
    fEvent->fEventHeader = GlobalManager::Instance()->GetEventHeader();
}

/*!
 * Inserts event into processing queue. Since its a unique ptr, no need to worry about ownership.
 * This should be called at the end of event action.
 */
void OutputManager::FinishAndSubmitEvent() {
    if (IsEmptyEvent()) return;

    spdlog::debug("OutputManager::FinishAndSubmitEvent");
    // print useful end of event info

    if (IsValidEvent()) {
        G4String energyWithUnits = G4BestUnit(fEvent->fSensitiveVolumesTotalEnergy * CLHEP::keV, "Energy");
        size_t numberOfSteps = 0;
        for (const auto& track : fEvent->fTracks) {
            numberOfSteps += track.fHits.fN;
        }
        spdlog::info(
            "OutputManager::FinishAndSubmitEvent - EventID {} - SubEventID {} - Sensitive volume energy: {} - Number of tracks: {} - Number of "
            "steps: {}",
            fEvent->fEventID, fEvent->fSubEventID, energyWithUnits, fEvent->fTracks.size(), numberOfSteps);
        RemoveUnwantedTracks();
        size_t numberOfInsertedEvents = GlobalManager::Instance()->InsertEvent(fEvent);
        spdlog::debug("OutputManager::FinishAndSubmitEvent - Added valid event");
    }
    UpdateEvent();
}

/*!
 * Inserts track into current event.
 * This should be called from tracking action at the start of tracking
 * The track can be updated afterwards i.e. with secondary information
 */
void OutputManager::RecordTrack(const G4Track* track) {
    // spdlog::debug("OutputManager::RecordTrack - Track ID {}", track->GetTrackID());
    bool inserted = fEvent->InsertTrack(track);

    if (inserted && fEvent->fSubEventID > 0) {
        const auto& lastTrack = fEvent->fTracks.back();
        assert(lastTrack.fTrackID == track->GetTrackID());
        bool isSubEventPrimary = fEvent->IsTrackSubEventPrimary(lastTrack.fTrackID);
        if (isSubEventPrimary) {
            spdlog::debug(
                "OutputManager::RecordTrack - Setting track ID {} as SubEventPrimaryTrack of EventID {} (SubEventID {}). Track info: {} - Created by "
                "{} - ParentID: {}",
                lastTrack.fTrackID, fEvent->fEventID, fEvent->fSubEventID, lastTrack.fParticleName, lastTrack.fCreatorProcess, lastTrack.fParentID);
        }
    }
}

/*!
 * Updates latest track with never information only available at the end of tracking, such as secondaries
 * It should be called at the end of tracking action
 */
void OutputManager::UpdateTrack(const G4Track* track) { fEvent->UpdateTrack(track); }

/*!
 * Inserts step into current event, at the end of the current track
 * This should be called at stepping action (only once!)
 */
void OutputManager::RecordStep(const G4Step* step) {
    spdlog::debug("OutputManager::RecordStep - Step for trackID {}", step->GetTrack()->GetTrackID());
    fEvent->InsertStep(step);
}

bool OutputManager::IsEmptyEvent() const { return !fEvent || fEvent->fTracks.empty(); }

bool OutputManager::IsValidEvent() const {
    if (IsEmptyEvent()) return false;
    if (GlobalManager::Instance()->GetSimulationConfig().fSaveAllEvents) return true;
    if (fEvent->fPrimaryParticleName == "geantino") return true;
    if (fEvent->fSensitiveVolumesTotalEnergy <= 0) return false;
    return true;
}

void OutputManager::AddSensitiveEnergy(Double_t energy, const TString& physicalVolumeName) {
    fEvent->fSensitiveVolumesTotalEnergy += energy;

    const TString physicalVolumeNameNew = fEvent->fEventHeader->fGeant4GeometryInfo->GetAlternativeNameFromGeant4PhysicalName(physicalVolumeName);

    bool sensitiveVolumeFound = false;
    for (int i = 0; i < fEvent->fSensitiveVolumeEnergy.size(); i++) {
        if (fEvent->fSensitiveVolumeName[i].EqualTo(physicalVolumeNameNew)) {
            fEvent->fSensitiveVolumeEnergy[i] += energy;
            sensitiveVolumeFound = true;
            break;
        }
    }
    if (!sensitiveVolumeFound) {
        fEvent->fSensitiveVolumeName.emplace_back(physicalVolumeNameNew);
        auto ID = fEvent->fEventHeader->fGeant4GeometryInfo->GetIDFromVolumeName(physicalVolumeName);
        fEvent->fSensitiveVolumeID.emplace_back(ID);
        fEvent->fSensitiveVolumeEnergy.emplace_back(energy);
    }
}

void OutputManager::RemoveUnwantedTracks() {
    const auto& config = GlobalManager::Instance()->GetSimulationConfig();
    if (config.fDetectorConfig.fKeepOnlyTracksInTheseVolumesList.empty()) {
        return;
    }
    if (fKeepOnlyTracksInTheseVolumesListAfterProcessing.empty()) {
        // Check volumes are OK
        const auto& geometryInfo = fEvent->fEventHeader->GetGeant4GeometryInfo();
        for (const auto& volumeString : config.fDetectorConfig.fKeepOnlyTracksInTheseVolumesList) {
            if (geometryInfo->IsValidPhysicalVolume(volumeString)) {
                fKeepOnlyTracksInTheseVolumesListAfterProcessing.insert(geometryInfo->GetAlternativeNameFromGeant4PhysicalName(volumeString));
            } else if (geometryInfo->IsValidLogicalVolume(volumeString)) {
                for (const auto& physicalVolumeName : geometryInfo->GetAllPhysicalVolumesFromLogical(volumeString)) {
                    fKeepOnlyTracksInTheseVolumesListAfterProcessing.insert(
                        geometryInfo->GetAlternativeNameFromGeant4PhysicalName(physicalVolumeName));
                }
            } else if (!geometryInfo->GetAllPhysicalVolumesMatchingExpression(volumeString).empty()) {
                spdlog::warn("OutputManager::RemoveUnwantedTracks - Using regex '{}' for physical volumes", volumeString);
                for (const auto& physicalVolumeName : geometryInfo->GetAllPhysicalVolumesMatchingExpression(volumeString)) {
                    fKeepOnlyTracksInTheseVolumesListAfterProcessing.insert(
                        geometryInfo->GetAlternativeNameFromGeant4PhysicalName(physicalVolumeName));
                }
            } else if (!geometryInfo->GetAllLogicalVolumesMatchingExpression(volumeString).empty()) {
                spdlog::warn("OutputManager::RemoveUnwantedTracks - Using regex '{}' for logical volumes", volumeString);
                for (const auto& logicalVolumeName : geometryInfo->GetAllLogicalVolumesMatchingExpression(volumeString)) {
                    for (const auto& physicalVolumeName : geometryInfo->GetAllPhysicalVolumesFromLogical(logicalVolumeName)) {
                        fKeepOnlyTracksInTheseVolumesListAfterProcessing.insert(
                            geometryInfo->GetAlternativeNameFromGeant4PhysicalName(physicalVolumeName));
                    }
                }
            }

            else {
                spdlog::error("OutputManager::RemoveUnwantedTracks - Volume name '{}' not found in physical or logical store", volumeString);
                exit(1);
            }
        }
        if (fKeepOnlyTracksInTheseVolumesListAfterProcessing.empty()) {
            spdlog::error("OutputManager::RemoveUnwantedTracks - Called but user supplied volume strings did not match any volume in geometry");
            exit(1);
        }
        spdlog::info("OutputManager::RemoveUnwantedTracks - User requested volume strings:");
        for (const auto& volumeString : config.fDetectorConfig.fKeepOnlyTracksInTheseVolumesList) {
            spdlog::info("\t- {}", volumeString);
        }
        spdlog::info("OutputManager::RemoveUnwantedTracks - Resolved physical volumes:");
        for (const auto& physical : fKeepOnlyTracksInTheseVolumesListAfterProcessing) {
            spdlog::info("\t- {}", physical);
        }
    }

    set<int> trackIDsToKeep;
    for (int i = 0; i < fEvent->fTracks.size(); i++) {
        // We iterate in reverse order to reduce time
        const auto& track = fEvent->fTracks[fEvent->fTracks.size() - 1 - i];
        if (trackIDsToKeep.count(track.fTrackID) > 0) {
            continue;
        }
        // energy deposited in important volumes
        bool keep = false;
        for (int i = 0; i < track.fHits.fN; i++) {
            double energy = track.fHits.fEnergy[i];
            if (energy > 0) {
                TString volume = track.fHits.fVolumeName[i];
                if (fKeepOnlyTracksInTheseVolumesListAfterProcessing.count(volume) > 0) {
                    keep = true;
                    break;
                }
            }
        }
        if (keep) {
            Geant4Track trackIter = track;
            while (true) {
                trackIDsToKeep.insert(trackIter.fTrackID);
                if (trackIter.IsPrimaryTrack() || trackIDsToKeep.count(trackIter.fParentID) > 0 ||
                    (fEvent->fSubEventID > 0 && fEvent->IsTrackSubEventPrimary(trackIter.fTrackID))) {
                    break;
                }
                trackIter = fEvent->GetTrackByID(trackIter.fParentID);
            }
        }
    }

    const size_t numberOfTracksBefore = fEvent->fTracks.size();

    // TODO: there should be a faster way to do this without having to deep copy the tracks
    std::vector<Geant4Track> tracksAfterRemoval;
    for (const auto& track : fEvent->fTracks) {
        // we do this to preserver original order
        if (trackIDsToKeep.count(track.fTrackID) > 0) {
            tracksAfterRemoval.push_back(*(&fEvent->GetTrackByID(track.fTrackID)));
        }
    }

    fEvent->fTracks = tracksAfterRemoval;

    size_t numberOfSteps = 0;
    for (const auto& track : fEvent->fTracks) {
        numberOfSteps += track.fHits.fN;
    }
    spdlog::info("OutputManager::RemoveUnwantedTracks - Tracks before removal: {} -> Tracks after removal: {} ({} steps)", numberOfTracksBefore,
                 fEvent->fTracks.size(), numberOfSteps);
}
