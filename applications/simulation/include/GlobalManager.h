//
// Created by lobis on 11/25/2021.
//

#ifndef RADIATION_TRANSPORT_GLOBALMANAGER_H
#define RADIATION_TRANSPORT_GLOBALMANAGER_H

#include <DataEvent.h>
#include <SimulationConfig.h>
#include <SimulationGeometryInfo.h>
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

    size_t InsertEvent(std::unique_ptr<DataEvent>& event);

    void WriteEvents();
    void WriteEventsAndCloseFile();

    TString GetOutputFilename() const;

    void SetupFile();

    Long64_t GetEntries();

    inline TString GetEventTreeName() const { return fEventTreeName; }

    inline void SetSimulationConfig(const SimulationConfig& simulationConfig) { fSimulationConfig = simulationConfig; }
    inline SimulationConfig GetSimulationConfig() const { return fSimulationConfig; }

    std::shared_ptr<SimulationGeometryInfo> fGeometryInfo;

   private:
    GlobalManager();
    static GlobalManager* pinstance_;

    SimulationConfig fSimulationConfig;

    bool fInitialized = false;

    std::mutex fEventContainerMutex;

    TFile* fFile = nullptr;
    TTree* fEventTree = nullptr;

    const TString fEventTreeName = "EventTree";

    DataEvent fEvent;

    std::queue<std::unique_ptr<DataEvent> > fEventContainer;
};

#endif  // RADIATION_TRANSPORT_GLOBALMANAGER_H
