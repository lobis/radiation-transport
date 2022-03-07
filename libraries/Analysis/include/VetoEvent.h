//
// Created by lobis on 3/6/2022.
//

#ifndef RADIATION_TRANSPORT_VETOEVENT_H
#define RADIATION_TRANSPORT_VETOEVENT_H

#include <Rtypes.h>
#include <TString.h>
#include <TVector3.h>

#include <map>
#include <vector>

class VetoEvent {
    ClassDef(VetoEvent, 1);

   public:
    Int_t fRunID{};
    Int_t fEventID{};
    Int_t fSubEventID{};

    Int_t fN = 0;
    std::vector<TVector3> fPosition;
    std::vector<Double_t> fTimeGlobal;
    std::vector<Double_t> fEnergy;
    std::vector<TString> fVolumeName;
    std::vector<TString> fParticleName;
    std::vector<TString> fParticleType;
    std::vector<TString> fProcessName;
    std::vector<TString> fProcessType;

    std::vector<Double_t> fDistanceToDetector;

    std::vector<std::map<TString, bool>> fHitFlagCustom;

   public:
    inline VetoEvent() = default;
};

#endif  // RADIATION_TRANSPORT_VETOEVENT_H
