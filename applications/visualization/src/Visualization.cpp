//
// Created by lobis on 26/11/2021.
//

#include "Visualization.h"

#include <TF1.h>
#include <TGCanvas.h>
#include <TGDockableFrame.h>
#include <TRandom.h>
#include <TSystem.h>
#include <spdlog/spdlog.h>

#include <filesystem>

using namespace std;

namespace fs = std::filesystem;

ClassImp(Visualization);

void Visualization::Initialize() { fEventTree->Branch("fEvent", &fEvent); }

void Visualization::LoadGeometry() {
    if (!fFile) {
        spdlog::warn("file is not populated yet, please select a valid ROOT file");
        return;
    }
    spdlog::info("Printing contents of file: '{}'", fFile->GetName());
    fFile->ls();

    const char* geometryKey = "Geometry";
    const char* eventTreeKey = "EventTree";

    spdlog::debug("Getting key '{}' from file", geometryKey);

    fGeo = fFile->Get<TGeoManager>(geometryKey);
    if (!fGeo) {
        spdlog::warn("File '{}' does not have key '{}'", fFile->GetName(), geometryKey);
        return;
    }

    spdlog::debug("Getting key '{}' from file", eventTreeKey);
    fEventTree = fFile->Get<TTree>(eventTreeKey);
    if (!fEventTree) {
        spdlog::warn("File '{}' does not have key '{}'", fFile->GetName(), eventTreeKey);
        return;
    }

    // fGeo->GetTopVolume()->Draw("ogl");
}

void Visualization::OpenFile(const TString& filename) {
    if (gSystem->AccessPathName(filename, kFileExists)) {
        spdlog::warn("User selected file '{}' does not exist", filename);
        fFileDisplay->SetText("&No File Selected");
        return;
    }

    fFileDisplay->SetText(TString::Format("&%s", fs::path(filename.Data()).filename().c_str()));
    fFileDisplay->SetToolTipText(filename.Data());

    spdlog::info("Opening: '{}'", filename);

    delete fFile;
    fFile = new TFile(filename);
}

void Visualization::SelectFile() {
    fOpenFile->SetState(kButtonDown);
    static TString dir(".");
    TGFileInfo fi;
    const char* filetypes[] = {"ROOT files", "*.root", nullptr, nullptr};
    fi.fFileTypes = filetypes;
    fi.SetIniDir(dir);
    printf("fIniDir = %s\n", fi.fIniDir);
    new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fi);
    printf("Open file: %s (dir: %s)\n", fi.fFilename, fi.fIniDir);
    dir = fi.fIniDir;
    fOpenFile->SetState(kButtonUp);
    OpenFile(fi.fFilename);
}

Visualization::Visualization(const TGWindow* p, UInt_t w, UInt_t h) : TGMainFrame(p, w, h) {
    SetWindowName("Visualization");
    // Create a horizontal frame containing buttons
    fCframe = new TGCompositeFrame(this, 800, 600, kHorizontalFrame | kFixedWidth);

    fOpenFile = new TGTextButton(fCframe, "&Open File");
    fOpenFile->Connect("Clicked()", "Visualization", this, "SelectFile()");
    fCframe->AddFrame(fOpenFile, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));
    fOpenFile->SetToolTipText("Open ROOT file");

    fFileDisplay = new TGTextButton(fCframe, "&No File Selected");
    fFileDisplay->Connect("Clicked()", "Visualization", this, nullptr);
    fCframe->AddFrame(fFileDisplay, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));

    fLoadGeometry = new TGTextButton(fCframe, "&Load Geometry");
    fLoadGeometry->Connect("Clicked()", "Visualization", this, "LoadGeometry()");
    fCframe->AddFrame(fLoadGeometry, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));
    fLoadGeometry->SetToolTipText("Load Geometry from current TFile");

    AddFrame(fCframe, new TGLayoutHints(kLHintsCenterX, 2, 2, 5, 1));

    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
}

Visualization::~Visualization() {
    spdlog::info("Visualization::~Visualization");

    fCframe->Cleanup();
    Cleanup();

    spdlog::info("Clean Exit");
}
