//
// Created by lobis on 28/11/2021.
//

#include <DataEvent.h>
#include <DataSteps.h>
#include <DataTrack.h>
#include <TEveManager.h>
#include <TEveStraightLineSet.h>

Color_t GetParticleColor(const TString& particleName) {
    Color_t color = 5;  // default, yellow
    if (particleName == "neutron") {
        color = 19;  // white
    } else if (particleName == "gamma") {
        color = 3;  // green
    } else if (particleName == "e-") {
        color = 2;  // red
    }
    return color;
}

TEveStraightLineSet* DataTrack::GetEveDrawable() const {
    auto lineSet = new TEveStraightLineSet(TString::Format("ID%d | %s | Init KE: %0.2f keV", fTrackID, fParticleName.Data(), fInitialKineticEnergy));

    for (int i = 0; i < fSteps.fN - 1; i++) {
        lineSet->AddLine({fSteps.fPosition[i].x(), fSteps.fPosition[i].y(), fSteps.fPosition[i].z()},  //
                         {fSteps.fPosition[i + 1].x(), fSteps.fPosition[i + 1].y(), fSteps.fPosition[i + 1].z()});
        lineSet->SetLineColor(GetParticleColor(fParticleName));
    }

    return lineSet;
}

void DataEvent::Draw() {
    for (const auto& track : fTracks) {
        auto line = track.GetEveDrawable();
        gEve->AddElement(line);
    }
}