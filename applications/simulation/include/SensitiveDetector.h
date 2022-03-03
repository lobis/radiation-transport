
#ifndef RADIATION_TRANSPORT_SENSITIVEDETECTOR_H
#define RADIATION_TRANSPORT_SENSITIVEDETECTOR_H

#include <G4VSensitiveDetector.hh>

#include "OutputManager.h"

class G4TouchableHistory;

class SensitiveDetector : public G4VSensitiveDetector {
   public:
    explicit SensitiveDetector(const G4String& name, bool kill = false);
    virtual ~SensitiveDetector() = default;

    virtual void Initialize(G4HCofThisEvent*) {}
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory*);

    inline bool IsKill() const { return fKill; };

   private:
    OutputManager* fOutput;
    bool fKill;
};

#endif  // RADIATION_TRANSPORT_SENSITIVEDETECTOR_H
