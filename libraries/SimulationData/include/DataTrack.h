//
// Created by lobis on 23/11/2021.
//

#ifndef RADIATION_TRANSPORT_DATATRACK_H
#define RADIATION_TRANSPORT_DATATRACK_H

#include "DataSteps.h"

class G4Track;
class G4Step;

class DataTrack {
   public:
    Int_t fTrackID{};
    Int_t fParentID{};

    TString fParticleName;
    Int_t fParticleID;

    TString fParticleType;
    TString fParticleSubType;

    TString fCreatorProcess;
    Int_t fNumberOfSecondaries{};
    Double_t fInitialKineticEnergy{};
    Double_t fTrackLength{};

    Double_t fWeight{};
    DataSteps fSteps;

   public:
    inline DataTrack() = default;

    void Print() const;

    ClassDef(DataTrack, 0);

    /* Geant4 */
   public:
    explicit DataTrack(const G4Track*);  //!
    void UpdateTrack(const G4Track*);    //!
    void InsertStep(const G4Step*);      //!
};

#endif  // RADIATION_TRANSPORT_DATATRACK_H
