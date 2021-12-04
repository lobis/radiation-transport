//
// Created by lobis on 11/25/2021.
//

#include "GlobalManager.h"

#include <DetectorConstructionConfig.h>
#include <SimulationConfig.h>
#include <TF1.h>
#include <TGeoManager.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include <G4GDMLParser.hh>
#include <G4GeneralParticleSource.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4Threading.hh>

#include "DetectorConstruction.h"
#include "GlobalManager.h"

using namespace std;

GlobalManager* GlobalManager::pinstance_ = nullptr;

GlobalManager* GlobalManager::Instance() {
    if (!pinstance_) {
        pinstance_ = new GlobalManager();
    }
    return pinstance_;
}

GlobalManager::GlobalManager() {
    // Master thread should create the GlobalManager, worker threads should spawn after the manager is created
    if (!G4Threading::IsMasterThread()) {
        spdlog::error("GlobalManager: GlobalManager should never be created by worker thread");
        exit(1);
    }

    fEventHeader = new Geant4EventHeader();
}

GlobalManager::~GlobalManager() = default;

void GlobalManager::WriteEvents() {
    spdlog::debug("GlobalManager::WriteEvents");

    if (G4Threading::IsMultithreadedApplication()) {
        lock_guard<mutex> guard(fEventContainerMutex);
    }

    if (fEventContainer.empty()) {
        return;
    }

    const auto beforeNumberOfEvents = fEventTree->GetEntries();

    while (!fEventContainer.empty()) {
        fEvent = *fEventContainer.front();
        fEventTree->Fill();
        fEventContainer.pop();
    }

    // fEventTree->Write();
    spdlog::debug("GlobalManager::WriteEvents - Saved {} events into {} (total {} events)", fEventTree->GetEntries() - beforeNumberOfEvents,
                  fFile->GetName(), fEventTree->GetEntries());
}

TString GlobalManager::GetOutputFilename() const { return fSimulationConfig.GetOutputFileAbsolutePath(); }

void GlobalManager::SetupFile() {
    spdlog::info("GlobalManager::SetupFile");
    // master thread will always run first
    if (!G4Threading::IsMasterThread()) {
        spdlog::error("GlobalManager::SetupFile - Called outside main thread, exiting");
        exit(1);
        return;
    }
    if (!fSimulationConfig.fSave) {
        spdlog::info("GlobalManager::SetupFile - Saving events is disabled");
        return;
    }
    if (fSimulationConfig.fOutputFilename.empty()) {
        spdlog::error("GlobalManager::SetupFile - Output file is empty");
        exit(1);
        return;
    }
    // the master thread does not run the monte carlo calculations, it will wait for the worker threads to finish.
    spdlog::info("GlobalManager::SetupFile - Initializing GlobalManager for {} thread {}", (G4Threading::IsMasterThread() ? "master" : "worker"),
                 G4Threading::G4GetThreadId());

    TString filename = GetOutputFilename();

    if (!fInitialized) {
        spdlog::info("GlobalManager::SetupFile - Not initialized yet, saving geometry...");

        spdlog::info("Writing metadata and geometry to {}", filename);

        TFile file(filename, "RECREATE");

        // write geometry to file
        const string geometryGdmlFilename = fSimulationConfig.fDetectorConfig.GetGeometryAbsolutePath();

        TGeoManager* geoManager;
        geoManager = TGeoManager::Import(geometryGdmlFilename.c_str());
        if (!geoManager) {
            string tmpGeometryFilename = "/tmp/geometry.gdml";
            spdlog::warn(
                "GlobalManager::SetupFile - No TGeoManager found, probably geometry was not loaded from GDML,"
                "trying to save current geometry into '{}' and reading it back",
                tmpGeometryFilename);
            G4GDMLParser parser;
            auto detectorConstruction = (DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction();
            parser.Write(tmpGeometryFilename.c_str(), detectorConstruction->GetWorld());
            remove(tmpGeometryFilename.c_str());
            geoManager = TGeoManager::Import(tmpGeometryFilename.c_str());
        }
        if (!geoManager) {
            spdlog::error("GlobalManager::SetupFile - No TGeoManager found!");
            exit(1);
        }

        geoManager->Write("Geometry");

        delete geoManager;

        file.Close();

        fInitialized = true;

        spdlog::info("Finished writing to {}", filename);
    }

    spdlog::info("GlobalManager::SetupFile - Creating 'fFile' ({})", filename);
    fFile = new TFile(filename, "UPDATE");

    fEventTree = new TTree("tmp", "Event Tree");
    fEventTree->Branch("fEvent", &fEvent);
}

void GlobalManager::WriteEventsAndCloseFile() {
    spdlog::info("GlobalManager::WriteEventsAndCloseFile");
    if (!fFile) {
        return;
    }
    // Save remaining events
    WriteEvents();
    // Write event tree into file
    if (fFile && fEventTree && fEventTree->GetEntries() > 0) {
        auto backupCycleTree = fFile->Get<TTree>(fEventTreeName);
        if (backupCycleTree) {
            auto list = new TList();
            list->Add(backupCycleTree);
            list->Add(fEventTree);
            TTree* mergeTree = TTree::MergeTrees(list);
            spdlog::info("GlobalManager::WriteEventsAndCloseFile - Merging fEventTree ({} events) with previous TTree ({} events) - Total events: {}",
                         fEventTree->GetEntries(), backupCycleTree->GetEntries(), mergeTree->GetEntries());
            mergeTree->SetName(fEventTreeName);
            mergeTree->Write(nullptr, TObject::kOverwrite);
        } else {
            fEventTree->SetName(fEventTreeName);  // important to name it here, else merge won't work!
            fEventTree->Write();
            spdlog::info("GlobalManager::WriteEventsAndCloseFile - Total events: {} - Closing and deleting 'fFile' ({})", fEventTree->GetEntries(),
                         fFile->GetName());
        }
    }

    spdlog::info("Output file: '{}'", fFile->GetName());

    if (fFile) {
        fFile->Close();
        delete fFile;
    }
}

Long64_t GlobalManager::GetEntries() {
    if (!fEventTree) return 0;
    if (G4Threading::IsMultithreadedApplication()) {
        lock_guard<mutex> guard(fEventContainerMutex);
    }
    return fEventTree->GetEntries();
}

size_t GlobalManager::InsertEvent(std::unique_ptr<Geant4Event>& event) {
    fEventContainerMutex.lock();
    fEventContainer.push(std::move(event));
    auto size = fEventContainer.size();
    fEventContainerMutex.unlock();
    return size;
}
