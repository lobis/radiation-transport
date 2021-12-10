
#include "EventAction.h"

#include <spdlog/spdlog.h>

#include <G4Event.hh>
#include <G4EventManager.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4SDManager.hh>
#include <G4Threading.hh>
#include <memory>

#include "GlobalManager.h"
#include "OutputManager.h"

using namespace std;

EventAction::EventAction() {
    output = OutputManager::Instance();
    fTimer.Start();
}

void EventAction::BeginOfEventAction(const G4Event* event) {
    spdlog::debug("EventAction::BeginOfEventAction ---> Begin of event {}", event->GetEventID());
    output->UpdateEvent();

    auto eventID = event->GetEventID();
    int s = G4RunManager::GetRunManager()->GetNumberOfEventsToBeProcessed() / 100;
    if ((s > 0 && (eventID + 1) % s == 0) || fTimer.RealTime() > 10.0) {
        fTimer.Start();
        spdlog::info("EventAction::BeginOfEventAction - RunID: {} ---> Begin of event {}{} [{:03.2f}%] - Number of entries saved this run: {}",
                     G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID(), eventID,
                     G4Threading::IsMultithreadedApplication()
                         ? TString::Format(" (T%d of %d)", G4Threading::G4GetThreadId() + 1, G4Threading::GetNumberOfRunningWorkerThreads())
                         : "",
                     100 * float(eventID + 1) / static_cast<float>(G4RunManager::GetRunManager()->GetNumberOfEventsToBeProcessed()),
                     GlobalManager::Instance()->GetEntries());
    } else {
        fTimer.Continue();
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
