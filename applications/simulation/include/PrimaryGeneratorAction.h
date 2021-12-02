//
// Created by Luis on 6/30/2021.
//

#ifndef RADIATION_TRANSPORT_PRIMARYGENERATORACTION_H
#define RADIATION_TRANSPORT_PRIMARYGENERATORACTION_H

#include <SourceConfig.h>
#include <TF1.h>
#include <TRandom.h>

#include <G4GeneralParticleSource.hh>
#include <G4ParticleGun.hh>
#include <G4SingleParticleSource.hh>
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

    G4ParticleGun fGun;           // This object will generate primary vertex
    G4SingleParticleSource fSPS;  // used to initialize and sample geant4 built in distributions

    const SourceConfig fSourceConfig;

    G4ParticleDefinition* fParticle;

    double fEnergyScaleFactor = CLHEP::keV;
    double fPositionScaleFactor = CLHEP::mm;

    /* Distributions extracted from the G4SingleParticleSource instance */
    G4SPSAngDistribution* fAngularDistribution = nullptr;
    G4SPSEneDistribution* fEnergyDistribution = nullptr;
    G4SPSPosDistribution* fPositionDistribution = nullptr;

    bool fInitialized = false;

    TF1* fAngularDistributionCustomFunctionCDF = nullptr;
    TF1* fEnergyDistributionCustomFunctionCDF = nullptr;

    void Initialize();
};

#endif  // RADIATION_TRANSPORT_PRIMARYGENERATORACTION_H
