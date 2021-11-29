
#include <G4ParticleDefinition.hh>
#include <G4UserStackingAction.hh>
#include <globals.hh>
#include <set>

#ifndef SIMULATION_STACKINGACTION_H
#define SIMULATION_STACKINGACTION_H

class OutputManager;

class StackingAction : public G4UserStackingAction {
   public:
    StackingAction();
    ~StackingAction();

    G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);
    void NewStage();

    inline std::set<const G4ParticleDefinition*> GetParticlesToIgnore() const { return fParticlesToIgnore; }
    inline void AddParticleToIgnore(const G4ParticleDefinition* particle) { fParticlesToIgnore.insert(particle); }

   private:
    OutputManager* output;

    const bool fFullChain;
    const G4double fMaxAllowedLifetime;
    G4String fMaxAllowedLifetimeWithUnit;

    std::set<const G4ParticleDefinition*> fParticlesToIgnore;
};
#endif  // SIMULATION_STACKINGACTION_H
