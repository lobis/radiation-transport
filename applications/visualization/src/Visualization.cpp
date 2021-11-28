//
// Created by lobis on 26/11/2021.
//

#include "Visualization.h"

#include <TEveStraightLineSet.h>
#include <TF1.h>
#include <TGCanvas.h>
#include <TGDockableFrame.h>
#include <TGLTH3Composition.h>
#include <TPaveLabel.h>
#include <TRandom.h>
#include <TStopwatch.h>
#include <TSystem.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <iostream>

using namespace std;

namespace fs = std::filesystem;

ClassImp(Visualization);

void Visualization::DrawEvent(Int_t index) {
    if (!fEventTree || index >= fEventTree->GetEntries()) {
        spdlog::error("Visualization::DrawEvent - Event tree does not exist or entry is out of bounds");
        return;
    }

    fEventTree->GetEntry(index);

    spdlog::info("Visualization::DrawEvent - Index {} - EventID {} - Number of tracks: {}", index, fEvent.fEventID, fEvent.fTracks.size());

    for (const auto& track : fEvent.fTracks) {
        /*
        if (track.fInitialKineticEnergy < 1.0) {
            continue;
        }
        */
        auto line = track.GetEveDrawable();
        fEveManager->AddElement(line);
    }

    // fViewer->GetClipSet()->SetClipType(TGLClip::EType(2));

    fEveManager->FullRedraw3D(kTRUE);
}

void Visualization::Test() { spdlog::warn("TEST!"); }

void Visualization::Update() {
    if (!fGeoManager) {
        spdlog::error("Visualization::Update - Geometry does not exist yet");
        exit(1);
    }

    const int transparencyLevel = fSliderTransparency->GetPosition();
    spdlog::info("Visualization::Update - Transparency: {}", transparencyLevel);
    for (int i = 0; i < fGeoManager->GetListOfVolumes()->GetEntries(); i++) {
        auto volume = fGeoManager->GetVolume(i);
        auto material = volume->GetMaterial();
        if (material->GetDensity() <= 0.01) {
            volume->SetTransparency(95);
            if (material->GetDensity() <= 0.001) {
                // We consider this vacuum for display purposes
                volume->SetVisibility(kFALSE);
            }
        } else {
            volume->SetTransparency(transparencyLevel);
        }
    }

    DrawEvent(fComboBoxEventID->GetSelected());

    fEveManager->FullRedraw3D(kFALSE);
}

void Visualization::Initialize() {}

void Visualization::LoadFile() {
    if (!fFile) {
        spdlog::warn("file is not populated yet, please select a valid ROOT file");
        return;
    }

    spdlog::info("Printing contents of file: '{}'", fFile->GetName());
    fFile->ls();

    const char* geometryKey = "Geometry";
    delete fGeoManager;
    fGeoManager = fFile->Get<TGeoManager>(geometryKey);
    if (!fGeoManager) {
        spdlog::warn("File '{}' does not have key '{}'", fFile->GetName(), geometryKey);
        return;
    }

    const char* eventTreeKey = "EventTree";
    spdlog::debug("Getting key '{}' from file", eventTreeKey);
    delete fEventTree;
    fEventTree = fFile->Get<TTree>(eventTreeKey);
    if (!fEventTree) {
        spdlog::warn("File '{}' does not have key '{}'", fFile->GetName(), eventTreeKey);
        return;
    }
    auto pEvent = &fEvent;
    fEventTree->SetBranchAddress("fEvent", &pEvent);

    fComboBoxEventID->RemoveAll();
    for (int i = 0; i < fEventTree->GetEntries(); i++) {
        spdlog::info("Added entry: {}", i);
        fEventTree->GetEntry(i);
        spdlog::info("EventID: {}", fEvent.fEventID);
        fComboBoxEventID->AddEntry(TString::Format("%d | EventID: %d", i, fEvent.fEventID), i);
    }
    const Int_t initialEntry = 0;
    fComboBoxEventID->Select(initialEntry);
    fEventTree->GetEntry(initialEntry);

    fEvent.Print();

    if (!fEveManager || !fEveManager->GetMainWindow()) {
        TStopwatch timer;
        timer.Start();
        spdlog::warn("Initializing Eve, this may take a while...");
        fEveManager = TEveManager::Create();
        timer.Stop();
        fViewer = fEveManager->GetDefaultGLViewer();
        spdlog::info("Initialized Eve in {:0.2f} seconds", timer.RealTime());
    }

    auto node = fGeoManager->GetTopNode();
    delete fEveGeoTopNode;
    fEveGeoTopNode = new TEveGeoTopNode(fGeoManager, node);
    fEveGeoTopNode->SetVisLevel(5);
    fEveManager->AddGlobalElement(fEveGeoTopNode);

    Update();  // for transparency

    for (const auto& track : fEvent.fTracks) {
        if (track.fInitialKineticEnergy < 1.0) {
            continue;
        }
        auto line = track.GetEveDrawable();
        fEveManager->AddElement(line);
    }

    // fViewer->GetClipSet()->SetClipType(TGLClip::EType(2));
    fViewer->CurrentCamera().Reset();

    fEveManager->FullRedraw3D(kTRUE);
}

void Visualization::OpenFile(const TString& filename) {
    if (gSystem->AccessPathName(filename, kFileExists)) {
        spdlog::warn("User selected file '{}' does not exist", filename);
        fTextEntryFile->SetText("No File Selected");
        return;
    }

    fTextEntryFile->SetText(TString::Format("%s", fs::path(filename.Data()).filename().c_str()));
    fTextEntryFile->SetToolTipText(filename.Data());

    spdlog::info("Opening: '{}'", filename);

    delete fFile;
    fFile = new TFile(filename);

    LoadFile();
}

void Visualization::SelectFile() {
    fTextButtonLoadFile->SetState(kButtonDown);
    static TString dir(".");
    TGFileInfo fi;
    const char* filetypes[] = {"ROOT files", "*.root", nullptr, nullptr};
    fi.fFileTypes = filetypes;
    fi.SetIniDir(dir);
    // printf("fIniDir = %s\n", fi.fIniDir);
    new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fi);
    // printf("Open file: %s (dir: %s)\n", fi.fFilename, fi.fIniDir);
    dir = fi.fIniDir;
    fTextButtonLoadFile->SetState(kButtonUp);
    OpenFile(fi.fFilename);
}

Visualization::Visualization(const TGWindow* p, UInt_t w, UInt_t h) : TGMainFrame(p, w, h) {
    SetWindowName("Visualization");
    // Create a horizontal frame containing buttons
    fCframe = new TGCompositeFrame(this, 800, 600, kHorizontalFrame | kFixedWidth);

    fTextButtonLoadFile = new TGTextButton(fCframe, "&Open File");
    fTextButtonLoadFile->Connect("Clicked()", "Visualization", this, "SelectFile()");
    fCframe->AddFrame(fTextButtonLoadFile, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));
    fTextButtonLoadFile->SetToolTipText("Open ROOT file");

    fTextEntryFile = new TGTextEntry(fCframe, "&No File Selected");
    fTextEntryFile->SetEditDisabled(true);
    fCframe->AddFrame(fTextEntryFile, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));

    fComboBoxEventID = new TGComboBox(fCframe);
    fComboBoxEventID->Resize(150, 20);
    fCframe->AddFrame(fComboBoxEventID, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));

    fSliderTransparency = new TGHSlider(fCframe, 150, kSlider1 | kScaleDownRight);
    fSliderTransparency->SetRange(0, 100);
    fSliderTransparency->SetPosition(70);
    fSliderTransparency->Connect("Clicked()", "Visualization", this, "Test()");
    fCframe->AddFrame(fSliderTransparency, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));

    fTextButtonUpdate = new TGTextButton(fCframe, "&Update");
    fTextButtonUpdate->Connect("Clicked()", "Visualization", this, "Update()");
    fCframe->AddFrame(fTextButtonUpdate, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));
    fTextButtonUpdate->SetToolTipText("Update TEveManager");

    AddFrame(fCframe, new TGLayoutHints(kLHintsCenterX, 2, 2, 5, 1));

    // fCanvas = new TCanvas("canvas", "Main Canvas", 200, 10, 700, 500);

    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
}

Visualization::~Visualization() {
    spdlog::info("Visualization::~Visualization");

    fCframe->Cleanup();
    Cleanup();

    spdlog::info("Clean Exit");

    exit(0);
}
