//
// Created by lobis on 26/11/2021.
//

#ifndef RADIATION_TRANSPORT_VISUALIZATION_H
#define RADIATION_TRANSPORT_VISUALIZATION_H

#include <TCanvas.h>
#include <TEveBrowser.h>
#include <TEveGeoNode.h>
#include <TEveManager.h>
#include <TEveStraightLineSet.h>
#include <TFile.h>
#include <TGButton.h>
#include <TGClient.h>
#include <TGComboBox.h>
#include <TGFileBrowser.h>
#include <TGFileDialog.h>
#include <TGFrame.h>
#include <TGLViewer.h>
#include <TGMenu.h>
#include <TGSlider.h>
#include <TGTextEntry.h>
#include <TGeoManager.h>
#include <TTree.h>

#include "Geant4Event.h"

class Visualization {
   private:
    TFile* fFile = nullptr;
    TTree* fEventTree = nullptr;

    TGeoManager* fGeoManager = nullptr;
    TEveManager* fEveManager = nullptr;
    TEveGeoTopNode* fEveGeoTopNode = nullptr;
    TGLViewer* fViewer = nullptr;

    Geant4Event* fEvent = nullptr;

    TGComboBox* fComboBoxEventID;
    TGHSlider* fSliderTransparency;

    TGTextButton* fTextButtonLoadFile;
    const TString fTextButtonLoadFileDefaultText = "No File Selected";

    void AddEveGUI();
    void DrawEvent(Long64_t);

    Long64_t fCurrentEventIndex;

    std::map<Long64_t, Long64_t> fEventIDs;
    void UpdateEventIDsComboBox();

   public:
    Visualization();
    ~Visualization();

    void Start();
    void OpenFile(const TString&);

    void Update();
    void SelectFile();

    void NextEvent();
    void PreviousEvent();

    ClassDef(Visualization, 1);
};

#endif  // RADIATION_TRANSPORT_VISUALIZATION_H
