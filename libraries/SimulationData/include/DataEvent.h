//
// Created by lobis on 23/11/2021.
//

#ifndef RADIATION_TRANSPORT_DATAEVENT_H
#define RADIATION_TRANSPORT_DATAEVENT_H

#include "DataTrack.h"

class G4Event;
class G4Track;
class G4Step;

class DataEvent {
   public:
    Int_t fRunID{};
    Int_t fEventID{};
    Int_t fSubEventID{};
    TString fInfo = "";

    Double_t fSensitiveVolumesTotalEnergy{};
    std::map<TString, Double_t> fSensitiveVolumeEnergy{};

    TString fPrimaryParticleName = "";
    Double_t fPrimaryEnergy{};
    TVector3 fPrimaryPosition{};
    TVector3 fPrimaryMomentum{};

    TString fSubEventPrimaryParticleName = "";
    Double_t fSubEventPrimaryEnergy{};
    TVector3 fSubEventPrimaryPosition{};
    TVector3 fSubEventPrimaryMomentum{};

    std::vector<DataTrack> fTracks{};

   public:
    inline DataEvent() = default;

    inline Int_t GetSubEventID() const { return fSubEventID; }
    inline void SetSubEventID(Int_t subEventID) { fSubEventID = subEventID; }

    ClassDefInline(DataEvent, 1);

   private:
    DataSteps fInitialStep;  //!

    /* Geant4 */
   public:
    explicit DataEvent(const G4Event*);  //!

    void InsertTrack(const G4Track*);  //!
    void UpdateTrack(const G4Track*);  //!
    void InsertStep(const G4Step*);    //!

   private:
    static bool IsValid(const G4Track*);  //!
    static bool IsValid(const G4Step*);   //!
};

#endif  // RADIATION_TRANSPORT_DATAEVENT_H
