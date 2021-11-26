//
// Created by lobis on 26/11/2021.
//

#include "Viewer.h"

#include <DataEvent.h>
#include <TFile.h>
#include <spdlog/spdlog.h>

using namespace std;

void Viewer::LoadFile(const TString& filename) {
    spdlog::info("Reading ROOT file: {}", filename);

    fFile = new TFile(filename);

    fGeo = fFile->Get<TGeoManager>("Geometry");

    if (!fGeo) {
        spdlog::error("File '{}' does not have key 'Geometry'", fFile->GetName());
        exit(1);
    }
}

void Viewer::Start() { fEve = TEveManager::Create(); }
