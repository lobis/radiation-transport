//
// Created by lobis on 23/11/2021.
//

#ifndef RADIATION_TRANSPORT_DATASTEPS_H
#define RADIATION_TRANSPORT_DATASTEPS_H

#include <Rtypes.h>
#include <TString.h>
#include <TVector3.h>

#include <map>
#include <vector>

class G4Step;

class TEvePointSet;

class DataSteps {
   public:
    Int_t fN = 0;
    std::vector<Int_t> fStepID;
    std::vector<TVector3> fPosition;
    std::vector<TVector3> fMomentumDirection;
    std::vector<Double_t> fTimeGlobal;
    std::vector<Double_t> fEnergy;
    std::vector<Double_t> fKineticEnergy;
    std::vector<Double_t> fKineticEnergyPost;
    std::vector<Double_t> fLength;
    std::vector<TString> fVolumeName;
    std::vector<TString> fVolumeNamePost;
    std::vector<Int_t> fVolumeID;
    std::vector<TString> fProcessName;
    std::vector<Int_t> fProcessID;
    std::vector<TString> fProcessType;
    std::vector<TString> fTargetNucleus;

   public:
    inline DataSteps() = default;

    void Print() const;

    ClassDef(DataSteps, 1);

    /* Geant4 */
   public:
    void InsertStep(const G4Step*);

    /* Visualization */
   public:
    TEvePointSet GetEveDrawable() const;
};

#endif  // RADIATION_TRANSPORT_DATASTEPS_H
