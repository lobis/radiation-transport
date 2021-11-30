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

    G4ParticleDefinition* GetParticle() const;
    double GetEnergy() const;
    G4ThreeVector GetDirection() const;
    G4ThreeVector GetPosition() const;

   private:
    OutputManager* fOutput;
    G4ParticleGun fGun;
    const SourceConfig fSourceConfig;

    G4ParticleDefinition* fParticle;

    double fEnergyScaleFactor = 1.0;

    std::unique_ptr<G4SPSAngDistribution> fAngularDistribution = nullptr;
    std::unique_ptr<G4SPSEneDistribution> fEnergyDistribution = nullptr;
    std::unique_ptr<G4SPSPosDistribution> fPositionDistribution = nullptr;

    bool fInitialized = false;

    void Initialize();
};

#endif  // RADIATION_TRANSPORT_PRIMARYGENERATORACTION_H
