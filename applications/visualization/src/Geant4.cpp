//
// Created by lobis on 28/11/2021.
//

#include <Geant4Event.h>
#include <Geant4Hits.h>
#include <Geant4Track.h>
#include <TEveManager.h>
#include <TEveStraightLineSet.h>

/*
enum EColor { kWhite =0,   kBlack =1,   kGray=920,
              kRed   =632, kGreen =416, kBlue=600, kYellow=400, kMagenta=616, kCyan=432,
              kOrange=800, kSpring=820, kTeal=840, kAzure =860, kViolet =880, kPink=900 };
*/

Color_t GetParticleColor(const TString& particleName) {
    Color_t color = kWhite;  // default
    // color based on charge
    if (particleName.Contains('-')) {
        color = kMagenta;  // red
    } else if (particleName.Contains('+')) {
        color = kAzure;
    }
    // color based on particle (overrides charge color)
    if (particleName == "neutron") {
        color = kOrange;  // white
    } else if (particleName == "gamma") {
        color = kGreen;  // green
    } else if (particleName == "e-") {
        color = kRed;  // red
    }
    return color;
}

Width_t GetLineWidth(Double_t energy) {
    /* log scale */
    Width_t width = TMath::Log10(energy / 100);
    width = (width > 10 ? 10 : width);
    width = (width < 1 ? 1 : width);
    return TMath::Log10(energy / 10);
}

TEveStraightLineSet* Geant4Track::GetEveDrawable() const {
    auto lineSet = new TEveStraightLineSet(TString::Format("ID%d | %s | Init KE: %0.2f keV", fTrackID, fParticleName.Data(), fInitialKineticEnergy));

    for (int i = 0; i < fSteps.fN - 1; i++) {
        lineSet->AddLine({static_cast<float>(fSteps.fPosition[i].x()), static_cast<float>(fSteps.fPosition[i].y()),
                          static_cast<float>(fSteps.fPosition[i].z())},  //
                         {static_cast<float>(fSteps.fPosition[i + 1].x()), static_cast<float>(fSteps.fPosition[i + 1].y()),
                          static_cast<float>(fSteps.fPosition[i + 1].z())});
        lineSet->SetLineColor(GetParticleColor(fParticleName));
        lineSet->SetLineWidth(GetLineWidth(fInitialKineticEnergy));
    }

    return lineSet;
}

void Geant4Event::Draw() {
    for (const auto& track : fTracks) {
        auto line = track.GetEveDrawable();
        gEve->AddElement(line);
    }
}