//
// Created by lobis on 26/11/2021.
//

#ifndef RADIATION_TRANSPORT_VIEWER_H
#define RADIATION_TRANSPORT_VIEWER_H

#include <TEveManager.h>
#include <TGeoManager.h>

class Viewer {
   public:
    inline Viewer() = default;

    void LoadFile(const TString&);
    void Start();

   private:
    TFile* fFile;
    TEveManager* fEve;
    TGeoManager* fGeo;
};

#endif  // RADIATION_TRANSPORT_VIEWER_H
