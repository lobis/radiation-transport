
#include "EventAction.h"

#include <G4Event.hh>
#include <G4EventManager.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4SDManager.hh>
#include <G4Threading.hh>
#include <memory>

#include "GlobalManager.h"
#include "OutputManager.h"
#include "spdlog/spdlog.h"

using namespace std;

EventAction::EventAction() { output = OutputManager::Instance(); }

void EventAction::BeginOfEventAction(const G4Event* event) {
    spdlog::debug("EventAction::BeginOfEventAction ---> Begin of event {}", event->GetEventID());

    output->UpdateEvent();

    auto eventID = event->GetEventID();
    int s = G4RunManager::GetRunManager()->GetNumberOfEventsToBeProcessed() / 100;
    if ((s > 0 && (eventID + 1) % s == 0)) {
        spdlog::info("EventAction::BeginOfEventAction - RunID: {} ---> Begin of event {} ({:03.2f}%) - Number of entries saved this run: {}",
                     G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID(), eventID,
                     100 * float(eventID + 1) / static_cast<float>(G4RunManager::GetRunManager()->GetNumberOfEventsToBeProcessed()),
                     GlobalManager::Instance()->GetEntries());
    }
}

void EventAction::EndOfEventAction(const G4Event* event) {
    spdlog::debug("EventAction::EndOfEventAction <--- End of event {}", event->GetEventID());

    // TODO: compute the sensitive volume energy etc from the sensitive detector instead of the output
    // G4SDManager* SDManager = G4SDManager::GetSDMpointer();

    output->FinishAndSubmitEvent();

    if (!G4Threading::IsMultithreadedApplication() ||  //
        (G4Threading::IsMultithreadedApplication() && G4Threading::G4GetThreadId() == 0)) {
        GlobalManager::Instance()->WriteEvents();
    }
}
