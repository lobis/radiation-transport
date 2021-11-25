//
// Created by lobis on 11/25/2021.
//

#ifndef RADIATION_TRANSPORT_ACTIONINITIALIZATION_H
#define RADIATION_TRANSPORT_ACTIONINITIALIZATION_H

#include <G4VUserActionInitialization.hh>

class DetectorConstruction;
class G4VSteppingVerbose;

class ActionInitialization : public G4VUserActionInitialization {
   public:
    ActionInitialization();
    virtual ~ActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;

    virtual G4VSteppingVerbose* InitializeSteppingVerbose() const;
};

#endif  // RADIATION_TRANSPORT_ACTIONINITIALIZATION_H
