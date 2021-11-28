
#include "OutputManager.h"

#include <DataEvent.h>

#include <G4RunManager.hh>
#include <G4Threading.hh>
#include <G4Track.hh>
#include <G4UserEventAction.hh>

#include "GlobalManager.h"
#include "spdlog/spdlog.h"

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
        // exit(1);
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
    fEvent = make_unique<DataEvent>(event);
    fEvent->fSimulationGeometryInfo = GlobalManager::Instance()->fGeometryInfo;
}

/*!
 * Inserts event into processing queue. Since its a unique ptr, no need to worry about ownership.
 * This should be called at the end of event action.
 */
void OutputManager::FinishAndSubmitEvent() {
    if (IsEmptyEvent()) return;

    spdlog::debug("OutputManager::FinishAndSubmitEvent");
    // print useful end of event info
    spdlog::debug("OutputManager::FinishAndSubmitEvent - EventID {} - SubEventID {} - Sensitive volume energy: {:03.2f}", fEvent->fEventID,
                  fEvent->fSubEventID, fEvent->fSensitiveVolumesTotalEnergy);

    if (IsValidEvent()) {
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
    fEvent->InsertTrack(track);
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
    if (fEvent->fSensitiveVolumesTotalEnergy <= 0) return false;
    return true;
}

void OutputManager::AddSensitiveEnergy(Double_t energy, const TString& physicalVolumeName) {
    // 'physicalVolumeName' should be the same as the sensitive detector name
    fEvent->fSensitiveVolumesTotalEnergy += energy;

    const TString physicalVolumeNameNew = fEvent->fSimulationGeometryInfo->GetAlternativeNameFromGeant4PhysicalName(physicalVolumeName);

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
        auto ID = fEvent->fSimulationGeometryInfo->GetIDFromVolumeName(physicalVolumeName);
        fEvent->fSensitiveVolumeID.emplace_back(ID);
        fEvent->fSensitiveVolumeEnergy.emplace_back(energy);
    }
}
