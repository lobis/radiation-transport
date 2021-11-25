//
// Created by Luis on 6/30/2021.
//

#ifndef RADIATION_TRANSPORT_PRIMARYGENERATORACTION_H
#define RADIATION_TRANSPORT_PRIMARYGENERATORACTION_H

#include <G4GeneralParticleSource.hh>
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
    inline G4SingleParticleSource* GetCurrentSource() const { return fGPS->GetCurrentSource(); }

   private:
    OutputManager* output{};
    std::unique_ptr<G4GeneralParticleSource> fGPS;
};

#endif  // RADIATION_TRANSPORT_PRIMARYGENERATORACTION_H
