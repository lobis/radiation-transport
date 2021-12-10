//
// Created by lobis on 23/04/2021.
//

#ifndef SIMULATION_TRACKINGACTION_H
#define SIMULATION_TRACKINGACTION_H

#include <G4Track.hh>
#include <G4UserTrackingAction.hh>
#include <globals.hh>

class G4ParticleDefinition;
class OutputManager;

class TrackingAction : public G4UserTrackingAction {
   public:
    TrackingAction();
    ~TrackingAction() = default;

    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);

   private:
    OutputManager* fOutput;
};

#endif  // SIMULATION_TRACKINGACTION_H
