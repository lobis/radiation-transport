
#ifndef SIMULATION_STEPPINGACTION_H
#define SIMULATION_STEPPINGACTION_H

#include <G4Event.hh>
#include <G4EventManager.hh>
#include <G4UserSteppingAction.hh>
#include <G4ios.hh>
#include <globals.hh>

class G4Step;
class OutputManager;

class SteppingAction : public G4UserSteppingAction {
   public:
    SteppingAction();
    ~SteppingAction() override;

    void UserSteppingAction(const G4Step*) override;

    G4TrackVector* GetfSecondary();

   private:
    OutputManager* output;
};

#endif  // SIMULATION_STEPPINGACTION_H
