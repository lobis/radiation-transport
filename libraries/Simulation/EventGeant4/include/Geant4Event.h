//
// Created by lobis on 23/11/2021.
//

#ifndef RADIATION_TRANSPORT_GEANT4EVENT_H
#define RADIATION_TRANSPORT_GEANT4EVENT_H

#include "Geant4EventHeader.h"
#include "Geant4Track.h"

class G4Event;
class G4Track;
class G4Step;

class Geant4Event {
    ClassDef(Geant4Event, 1);

   public:
    Int_t fRunID{};
    Int_t fEventID{};
    Int_t fSubEventID{};

    Geant4EventHeader* fEventHeader = nullptr;  //!

    Double_t fSensitiveVolumesTotalEnergy{};
    std::vector<Double_t> fSensitiveVolumeEnergy{};
    std::vector<Int_t> fSensitiveVolumeID{};
    std::vector<TString> fSensitiveVolumeName{};

    TString fPrimaryParticleName = "";
    Double_t fPrimaryEnergy{};
    TVector3 fPrimaryPosition{};
    TVector3 fPrimaryMomentum{};

    TString fSubEventPrimaryParticleName = "";
    Double_t fSubEventPrimaryEnergy{};
    TVector3 fSubEventPrimaryPosition{};
    TVector3 fSubEventPrimaryMomentum{};

    std::vector<Geant4Track> fTracks{};

   public:
    inline Geant4Event() = default;

    inline Int_t GetSubEventID() const { return fSubEventID; }
    inline void SetSubEventID(Int_t subEventID) { fSubEventID = subEventID; }

    void Print() const;
    std::string ToJson() const;

    void PrintSensitiveInfo() const;

    const Geant4Track& GetTrackByID(int trackID) const;
    // Int_t GetTrackIndexByID(int trackID);
    bool IsTrackSubEventPrimary(int trackID);
    double GetEnergyInVolume(const TString& volume, const TString& processName = "") const;
    void InitializeEnergyInVolumeMap();
    std::map<TString, double> GetSensitiveEnergyInVolumes() const;

   private:
    std::map<int, int> fTrackIDToTrackIndex = {};      //!
    Geant4Hits fInitialStep;                           //!
    std::map<std::string, double> fEnergyInVolumeMap;  //!

    /* Geant4 */
   public:
    explicit Geant4Event(const G4Event*);  //!

    bool InsertTrack(const G4Track*);  //!
    void UpdateTrack(const G4Track*);  //!
    void InsertStep(const G4Step*);    //!

    /* Visualization */
    void Draw();
};

#endif  // RADIATION_TRANSPORT_GEANT4EVENT_H
