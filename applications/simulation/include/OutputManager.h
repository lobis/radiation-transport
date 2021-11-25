
#ifndef RADIATION_TRANSPORT_OUTPUTMANAGER_H
#define RADIATION_TRANSPORT_OUTPUTMANAGER_H

#include <DataEvent.h>
#include <TFile.h>
#include <TString.h>
#include <TTree.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <vector>

#include "globals.hh"

class G4Event;
class G4Track;
class G4Step;

class OutputManager {
   public:
    static OutputManager* Instance();
    inline ~OutputManager() = default;

    void UpdateEvent();
    void FinishAndSubmitEvent();

    inline int GetSubEventID() const {
        if (fEvent) return fEvent->GetSubEventID();
    }
    inline void SetSubEventID(Int_t subEventID) {
        if (fEvent) fEvent->SetSubEventID(subEventID);
    }

    void RecordTrack(const G4Track*);
    void UpdateTrack(const G4Track*);

    void RecordStep(const G4Step*);

    inline void AddSensitiveEnergy(double energy) { fEvent->fSensitiveVolumesTotalEnergy += energy; }

   private:
    inline OutputManager() = default;
    static thread_local OutputManager* pinstance_;

    std::unique_ptr<DataEvent> fEvent{};

    double fSensitiveEnergyTotal{};

    bool IsEmptyEvent() const;
    bool IsValidEvent() const;
};

#endif  // RADIATION_TRANSPORT_OUTPUTMANAGER_H
