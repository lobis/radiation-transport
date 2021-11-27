//
// Created by lobis on 26/11/2021.
//

#ifndef RADIATION_TRANSPORT_Analysis_H
#define RADIATION_TRANSPORT_Analysis_H

#include <TCanvas.h>
#include <TEveManager.h>
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

class Analysis : public TGMainFrame {
   private:
    inline Analysis() = default;

   public:
    virtual ~Analysis();

    ClassDef(Analysis, 1);
};

#endif  // RADIATION_TRANSPORT_Analysis_H
