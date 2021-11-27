//
// Created by lobis on 27/11/2021.
//

#ifndef RADIATION_TRANSPORT_VETOEVENT_H
#define RADIATION_TRANSPORT_VETOEVENT_H

#include <TString.h>
#include <TVector3.h>

#include <vector>

#include "DataEvent.h"

class VetoEvent {
   public:
    Int_t fRunID{};
    Int_t fEventID{};
    Int_t fSubEventID{};

    Int_t fNumberOfVetoes{};

    std::vector<Int_t> fVetoID;
    std::vector<TString> fVetoName;
    std::vector<TString> fVetoGroupName;
    std::vector<TString> fVetoSubGroupName;

    std::vector<TVector3> fVetoBoundaryPoint;
    std::vector<TVector3> fVetoBoundaryNormal;

    std::vector<Double_t> fVetoRawEnergy;
    std::vector<Double_t> fVetoRawAttenuatedEnergy;
    std::vector<Double_t> fVetoQuenchedEnergy;
    std::vector<Double_t> fVetoQuenchedAttenuatedEnergy;

    std::vector<Double_t> fVetoTriggerTimeOffset;

    std::vector<Double_t> fVetoQuenchingFactor;
    std::vector<Double_t> fVetoAttenuationLength;

    std::vector<std::vector<Double_t>> fVetoHitsRawEnergy;
    std::vector<std::vector<Double_t>> fVetoHitsTime;
    std::vector<std::vector<TVector3>> fVetoHitsPosition;
    std::vector<std::vector<Int_t>> fVetoHitsIsQuenched;  // Bool does not work for some reason

   public:
    ClassDef(VetoEvent, 1);
};

#endif  // RADIATION_TRANSPORT_VETOEVENT_H
