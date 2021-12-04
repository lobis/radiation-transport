//
// Created by lobis on 11/25/2021.
//

#ifndef RADIATION_TRANSPORT_GLOBALMANAGER_H
#define RADIATION_TRANSPORT_GLOBALMANAGER_H

#include <Geant4Event.h>
#include <SimulationConfig.h>
#include <TFile.h>
#include <TTree.h>

#include <globals.hh>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

#include "spdlog/spdlog.h"

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
    inline SimulationConfig GetSimulationConfig() const { return fSimulationConfig; }

    Geant4GeometryInfo* fGeometryInfo = nullptr;  // std::shared_ptr<Geant4GeometryInfo>

   private:
    GlobalManager();
    static GlobalManager* pinstance_;

    SimulationConfig fSimulationConfig;

    bool fInitialized = false;

    std::mutex fEventContainerMutex;

    TFile* fFile = nullptr;
    TTree* fEventTree = nullptr;

    const TString fEventTreeName = "EventTree";

    Geant4Event fEvent;

    std::queue<std::unique_ptr<Geant4Event> > fEventContainer;
};

#endif  // RADIATION_TRANSPORT_GLOBALMANAGER_H
