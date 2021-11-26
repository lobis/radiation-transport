//
// Created by lobis on 26/11/2021.
//

#ifndef RADIATION_TRANSPORT_VISUALIZATION_H
#define RADIATION_TRANSPORT_VISUALIZATION_H

#include <TFile.h>
#include <TGButton.h>
#include <TGClient.h>
#include <TGFileBrowser.h>
#include <TGFileDialog.h>
#include <TGFrame.h>
#include <TGMenu.h>
#include <TGeoManager.h>
#include <TTree.h>

#include "DataEvent.h"

class Visualization : public TGMainFrame {
   private:
    TGCompositeFrame* fCframe;
    TGTextButton *fOpenFile, *fFileDisplay, *fLoadGeometry;

    TFile* fFile = nullptr;
    TGeoManager* fGeo = nullptr;

    TTree* fEventTree = nullptr;

    DataEvent fEvent;

   public:
    Visualization(const TGWindow* p, UInt_t w, UInt_t h);
    virtual ~Visualization();
    // slots
    void SelectFile();
    void OpenFile(const TString&);
    void LoadGeometry();

    void Initialize();

    ClassDef(Visualization, 0);
};

#endif  // RADIATION_TRANSPORT_VISUALIZATION_H
