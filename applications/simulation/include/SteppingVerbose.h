//
// Created by lobis on 17/09/2021.
//

#ifndef RADIATION_TRANSPORT_STEPPINGVERBOSE_H
#define RADIATION_TRANSPORT_STEPPINGVERBOSE_H

#include <G4SteppingManager.hh>
#include <G4SteppingVerbose.hh>
#include <G4VSteppingVerbose.hh>

class OutputManager;
class G4Step;

class SteppingVerbose : public G4SteppingVerbose {
   public:
    SteppingVerbose();
    ~SteppingVerbose();

    virtual void TrackingStarted();
    virtual void StepInfo();

    int GetSteppingVerbose() { return fManager->GetverboseLevel(); }
    void SetSteppingVerbose(int level) { fManager->SetVerboseLevel(level); }

   private:
    OutputManager* output;
};

#endif  // RADIATION_TRANSPORT_STEPPINGVERBOSE_H
