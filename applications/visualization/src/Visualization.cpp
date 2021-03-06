//
// Created by lobis on 26/11/2021.
//

#include "Visualization.h"

#include <TEveEventManager.h>
#include <TEveStraightLineSet.h>
#include <TEveViewer.h>
#include <TF1.h>
#include <TGCanvas.h>
#include <TGDockableFrame.h>
#include <TGLTH3Composition.h>
#include <TGTab.h>
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

void Visualization::AddEveGUI() {
    fEveManager->GetBrowser()->GetTabRight()->SetTab(1);

    TEveBrowser* browser = fEveManager->GetBrowser();
    browser->StartEmbedding(TRootBrowser::kLeft);

    auto pMainFrame = new TGMainFrame(gClient->GetRoot(), 1000, 600);
    pMainFrame->SetWindowName("Eve GUI");
    pMainFrame->SetCleanup(kDeepCleanup);

    {
        auto hf = new TGHorizontalFrame(pMainFrame);

        fTextButtonLoadFile = new TGTextButton(hf);
        fTextButtonLoadFile->SetText(fTextButtonLoadFileDefaultText);
        fTextButtonLoadFile->SetToolTipText(fTextButtonLoadFileDefaultText);
        fTextButtonLoadFile->Connect("Clicked()", "Visualization", this, "SelectFile()");
        fTextButtonLoadFile->Resize(150, 20);
        hf->AddFrame(fTextButtonLoadFile, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 5, 5, 5, 5));

        pMainFrame->AddFrame(hf);
    }

    {
        auto hf = new TGHorizontalFrame(pMainFrame);

        fComboBoxEventID = new TGComboBox(hf);
        fComboBoxEventID->Resize(150, 20);
        hf->AddFrame(fComboBoxEventID, new TGLayoutHints(kLHintsTop | kLHintsCenterX, 5, 5, 5, 5));

        pMainFrame->AddFrame(hf);
    }

    {
        auto hf = new TGHorizontalFrame(pMainFrame, kLHintsCenterX);

        TGPictureButton* b = nullptr;

        b = new TGPictureButton(hf, gClient->GetPicture("GoBack.gif"));
        hf->AddFrame(b, new TGLayoutHints(kLHintsTop | kLHintsLeft));
        b->Connect("Clicked()", "Visualization", this, "PreviousEvent()");

        b = new TGPictureButton(hf, gClient->GetPicture("refresh.png"));
        hf->AddFrame(b, new TGLayoutHints(kLHintsTop | kLHintsCenterX));
        b->Connect("Clicked()", "Visualization", this, "Update()");

        b = new TGPictureButton(hf, gClient->GetPicture("GoForward.gif"));
        hf->AddFrame(b, new TGLayoutHints(kLHintsTop | kLHintsRight));
        b->Connect("Clicked()", "Visualization", this, "NextEvent()");

        pMainFrame->AddFrame(hf);
    }

    {
        auto hf = new TGHorizontalFrame(pMainFrame);

        fSliderTransparency = new TGHSlider(hf, 150, kSlider1 | kScaleDownRight);
        fSliderTransparency->SetRange(0, 100);
        fSliderTransparency->SetPosition(70);
        hf->AddFrame(fSliderTransparency, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));

        pMainFrame->AddFrame(hf);
    }

    pMainFrame->MapSubwindows();
    pMainFrame->Resize();
    pMainFrame->MapWindow();

    browser->StopEmbedding();
    browser->SetTabTitle("Event Control", 0);
}

Visualization::Visualization() {}

Visualization::~Visualization() {}

void Visualization::Start() {
    fEveManager = TEveManager::Create();
    fViewer = fEveManager->GetDefaultGLViewer();
    fEveManager->AddEvent(new TEveEventManager("Event", "Event"));

    fViewer->SetStyle(TGLRnrCtx::kOutline);
    fEveManager->GetDefaultViewer()->SetName("Perspective");
    AddEveGUI();

    const std::map<TString, TGLViewer::ECameraType> projectionsMap = {
        {"Projection XY", TGLViewer::kCameraOrthoXnOY},  //
        {"Projection XZ", TGLViewer::kCameraOrthoXnOZ},  //
        {"Projection YZ", TGLViewer::kCameraOrthoZOY}    //
    };
    for (const auto& [name, cameraType] : projectionsMap) {
        auto slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
        auto pack = slot->MakePack();
        pack->SetElementName(name);
        pack->SetHorizontal();
        pack->SetShowTitleBar(kFALSE);
        pack->NewSlot()->MakeCurrent();
        auto f3DView = gEve->SpawnNewViewer("3D View", pack->GetName());
        f3DView->AddScene(gEve->GetGlobalScene());
        f3DView->AddScene(gEve->GetEventScene());
        f3DView->GetGLViewer()->SetCurrentCamera(cameraType);
    }

    fEveManager->GetViewers()->SwitchColorSet();
}

void Visualization::OpenFile(const TString& filename) {
    if (gSystem->AccessPathName(filename, kFileExists)) {
        spdlog::warn("User selected file '{}' does not exist", filename);
        fTextButtonLoadFile->SetText(fTextButtonLoadFileDefaultText);
        return;
    }

    fTextButtonLoadFile->SetText(TString::Format("%s", fs::path(filename.Data()).filename().c_str()));
    fTextButtonLoadFile->SetToolTipText(filename.Data());

    spdlog::info("Opening: '{}'", filename);

    delete fFile;
    fFile = new TFile(filename);

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

    fEventIDs.clear();
    UpdateEventIDsComboBox();

    fEventTree->SetBranchAddress("fEvent", &fEvent);

    fCurrentEventIndex = 0;
    fComboBoxEventID->Select(fCurrentEventIndex);
    fEventTree->GetEntry(fCurrentEventIndex);

    auto node = fGeoManager->GetTopNode();
    delete fEveGeoTopNode;
    fEveGeoTopNode = new TEveGeoTopNode(fGeoManager, node);
    fEveGeoTopNode->SetVisLevel(5);
    fEveManager->AddGlobalElement(fEveGeoTopNode);

    Update();  // for transparency

    // fViewer->GetClipSet()->SetClipType(TGLClip::EType(2));
    fViewer->CurrentCamera().Reset();

    fEveManager->FullRedraw3D(kTRUE);
}

void Visualization::SelectFile() {
    fTextButtonLoadFile->SetState(kButtonDown);
    static TString dir(".");
    TGFileInfo fileInfo;
    const char* filetypes[] = {"ROOT files", "*.root", nullptr, nullptr};
    fileInfo.fFileTypes = filetypes;
    fileInfo.SetIniDir(dir);
    new TGFileDialog(gClient->GetRoot(), gClient->GetRoot(), kFDOpen, &fileInfo);
    fTextButtonLoadFile->SetState(kButtonUp);
    OpenFile(fileInfo.fFilename);
}

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

    fCurrentEventIndex = fComboBoxEventID->GetSelected();
    DrawEvent(fCurrentEventIndex);

    fEveManager->FullRedraw3D(kFALSE);
}

void Visualization::DrawEvent(Long64_t index) {
    if (!fEventTree || index >= fEventTree->GetEntries()) {
        spdlog::error("Visualization::DrawEvent - Event tree does not exist or entry is out of bounds");
        return;
    }

    fEventTree->GetEntry(index);

    auto eventID = fEvent->fEventID;

    fEventIDs[index] = eventID;
    UpdateEventIDsComboBox();
    fComboBoxEventID->Select(index, false);

    spdlog::info("Visualization::DrawEvent - Index {} - EventID {} - Number of tracks: {}", index, fEvent->fEventID, fEvent->fTracks.size());

    fEveManager->GetViewers()->DeleteAnnotations();
    fEveManager->GetCurrentEvent()->DestroyElements();

    fEvent->Draw();

    fEveManager->FullRedraw3D(kFALSE);
}

void Visualization::UpdateEventIDsComboBox() {
    fComboBoxEventID->RemoveAll();
    for (int i = 0; i < fEventTree->GetEntries(); i++) {
        fComboBoxEventID->AddEntry(
            TString::Format("%d | EventID: %s", i, fEventIDs.count(i) > 0 ? TString::Format("%lld", fEventIDs[i]).Data() : "(?)"), i);
    }
}

void Visualization::NextEvent() {
    if (fCurrentEventIndex < fEventTree->GetEntries() - 1) {
        fCurrentEventIndex += 1;
        fComboBoxEventID->Select(fCurrentEventIndex, false);
        Update();
        return;
    }

    spdlog::warn("Visualization::NextEvent - Limit reached");
}

void Visualization::PreviousEvent() {
    if (fCurrentEventIndex > 0) {
        fCurrentEventIndex -= 1;
        fComboBoxEventID->Select(fCurrentEventIndex, false);
        Update();
        return;
    }

    spdlog::warn("Visualization::PreviousEvent - Limit reached");
}