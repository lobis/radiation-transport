//
// Created by lobis on 26/11/2021.
//

#ifndef RADIATION_TRANSPORT_VISUALIZATION_H
#define RADIATION_TRANSPORT_VISUALIZATION_H

#include <TCanvas.h>
#include <TEveGeoNode.h>
#include <TEveManager.h>
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

#include "DataEvent.h"

class Visualization : public TGMainFrame {
   private:
    inline Visualization() = default;

    TGCompositeFrame* fCframe;
    TGTextButton *fTextButtonLoadFile, *fTextButtonUpdate;
    TGTextEntry* fTextEntryFile;
    TGComboBox* fComboBoxEventID;
    TGHSlider* fSliderTransparency;
    TFile* fFile = nullptr;
    TGeoManager* fGeoManager = nullptr;
    TEveManager* fEveManager = nullptr;
    TEveGeoTopNode* fEveGeoTopNode = nullptr;
    TTree* fEventTree = nullptr;
    TGLViewer* fViewer = nullptr;

    DataEvent fEvent;

    TCanvas* fCanvas;

   public:
    Visualization(const TGWindow* p, UInt_t w, UInt_t h);
    virtual ~Visualization();
    // slots
    void SelectFile();
    void OpenFile(const TString&);
    void LoadFile();
    void Test();
    void Update();
    void DrawEvent(Int_t);

    void Initialize();

    ClassDef(Visualization, 1);
};

#endif  // RADIATION_TRANSPORT_VISUALIZATION_H
