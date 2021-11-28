
#ifndef SIMULATION_EVENTACTION_H
#define SIMULATION_EVENTACTION_H

#include <TStopwatch.h>

#include <G4UserEventAction.hh>
#include <globals.hh>
#include <memory>

class G4Track;
class G4Step;
class OutputManager;

class EventAction : public G4UserEventAction {
   public:
    EventAction();
    inline ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

   private:
    OutputManager* output;

    TStopwatch fTimer;
};

#endif  // SIMULATION_EVENTACTION_H
