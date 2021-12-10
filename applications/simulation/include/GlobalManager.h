//
// Created by lobis on 11/25/2021.
//

#ifndef RADIATION_TRANSPORT_GLOBALMANAGER_H
#define RADIATION_TRANSPORT_GLOBALMANAGER_H

#include <Geant4Event.h>
#include <SimulationConfig.h>
#include <TFile.h>
#include <TTree.h>
#include <spdlog/spdlog.h>

#include <ctime>
#include <globals.hh>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

class GlobalManager {
   public:
    static GlobalManager* Instance();
    ~GlobalManager();

    size_t InsertEvent(std::unique_ptr<Geant4Event>& event);

    void WriteEvents();
    void WriteEventsAndCloseFile();

    TString GetOutputFilename() const;

    void SetupFile();

    Long64_t GetEntries();

    inline TString GetEventTreeName() const { return fEventTreeName; }

    inline void SetSimulationConfig(const SimulationConfig& simulationConfig) { fSimulationConfig = simulationConfig; }
    inline const SimulationConfig& GetSimulationConfig() const { return fSimulationConfig; }
    static inline void SetRunTimestamp() { fEventHeader.fTimestamp = std::time(nullptr); }
    static inline Geant4EventHeader fEventHeader;

   private:
    GlobalManager();
    static GlobalManager* pinstance_;

    SimulationConfig fSimulationConfig;

    bool fInitialized = false;

    std::mutex fEventContainerMutex;

    TFile* fFile = nullptr;
    TTree* fEventTree = nullptr;
    // TTree* fConfigTree = nullptr;

    const TString fEventTreeName = "EventTree";

    Geant4Event fEvent;

    std::queue<std::unique_ptr<Geant4Event> > fEventContainer;
};

#endif  // RADIATION_TRANSPORT_GLOBALMANAGER_H
