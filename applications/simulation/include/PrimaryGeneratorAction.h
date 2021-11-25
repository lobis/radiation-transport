//
// Created by Luis on 6/30/2021.
//

#ifndef RADIATION_TRANSPORT_PRIMARYGENERATORACTION_H
#define RADIATION_TRANSPORT_PRIMARYGENERATORACTION_H

#include <SourceConfig.h>

#include <G4GeneralParticleSource.hh>
#include <G4ParticleGun.hh>
#include <G4ThreeVector.hh>
#include <G4VUserPrimaryGeneratorAction.hh>
#include <globals.hh>

class G4Event;
class OutputManager;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
   public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();
    virtual void GeneratePrimaries(G4Event* event);

   private:
    OutputManager* fOutput;
    G4ParticleGun fGun;
    const SourceConfig fSourceConfig;
};

#endif  // RADIATION_TRANSPORT_PRIMARYGENERATORACTION_H
