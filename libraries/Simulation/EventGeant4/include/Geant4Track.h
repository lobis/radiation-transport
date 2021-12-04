//
// Created by lobis on 23/11/2021.
//

#ifndef RADIATION_TRANSPORT_Geant4Track_H
#define RADIATION_TRANSPORT_Geant4Track_H

#include "Geant4Hits.h"

class G4Track;
class G4Step;

class TEveStraightLineSet;

class Geant4Track {
    ClassDef(Geant4Track, 1);

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
    Geant4Hits fSteps;

   public:
    inline Geant4Track() = default;

    void Print() const;

    /* Geant4 */
   public:
    explicit Geant4Track(const G4Track*);  //!
    void UpdateTrack(const G4Track*);    //!
    void InsertStep(const G4Step*);      //!

    /* Visualization */
   public:
    TEveStraightLineSet* GetEveDrawable() const;
};

#endif  // RADIATION_TRANSPORT_Geant4Track_H
