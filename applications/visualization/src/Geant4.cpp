//
// Created by lobis on 28/11/2021.
//

#include <TEveManager.h>
#include <TEvePointSet.h>
#include <TEveStraightLineSet.h>
//
#include <Geant4Event.h>
#include <Geant4Hits.h>
#include <Geant4Track.h>

#include "spdlog/spdlog.h"

using namespace std;

/*
enum EColor { kWhite =0,   kBlack =1,   kGray=920,
              kRed   =632, kGreen =416, kBlue=600, kYellow=400, kMagenta=616, kCyan=432,
              kOrange=800, kSpring=820, kTeal=840, kAzure =860, kViolet =880, kPink=900 };
*/

struct TrackVisualConfiguration {
    Color_t fColor = kWhite;
    Style_t fLineStyle = 1;
    Width_t fLineWidth = 1.0;
};

TrackVisualConfiguration GetTrackVisualConfiguration(const Geant4Track& track) {
    auto config = TrackVisualConfiguration();
    // color based on charge
    if (track.fParticleName.Contains('-')) {
        config.fColor = kMagenta;  // red
    } else if (track.fParticleName.Contains('+')) {
        config.fColor = kAzure;
    }
    // color based on particle (overrides charge color)
    if (track.fParticleName == "neutron") {
        config.fColor = kOrange;  // white
    } else if (track.fParticleName == "gamma") {
        config.fColor = kGreen;  // green
    } else if (track.fParticleName == "e-") {
        config.fColor = kRed;  // red
    }

    // width

    Width_t width = TMath::Log10(track.fInitialKineticEnergy / 100);
    width = (width > 10 ? 10 : width);
    width = (width < 1 ? 1 : width);
    config.fLineWidth = TMath::Log10(track.fInitialKineticEnergy / 10);

    // line style

    if (track.fCreatorProcess == "nCapture") {
        config.fLineStyle = 2;
    }

    return config;
}

struct HitsVisualConfiguration {
    Color_t fColor = kBlack;
    Style_t fMarkerStyle = 1;
    Size_t fMarkerSize = 1.0;
};

HitsVisualConfiguration GetHitsVisualConfiguration(const TString& processNameOrType) {
    auto config = HitsVisualConfiguration();
    // based on particle type
    if (processNameOrType.EqualTo("Electromagnetic")) {
        config.fColor = kRed;
    } else if (processNameOrType.EqualTo("Init")) {
        // custom process (not Geant4)
        config.fColor = kWhite;
    }

    // based on particle name
    if (processNameOrType.EqualTo("Transportation")) {
        config.fColor = kWhite;
    } else if (processNameOrType.EqualTo("Init")) {
        // custom process (not Geant4)
        config.fColor = kWhite;
    } else if (processNameOrType.EqualTo("hadElastic")) {
        config.fColor = kOrange;
    } else if (processNameOrType.EqualTo("neutronInelastic")) {
        config.fColor = kGreen;
        config.fMarkerStyle = 4;
    } else if (processNameOrType.EqualTo("nCapture")) {
        config.fColor = kBlue;
        config.fMarkerStyle = 2;
        config.fMarkerSize = 4.0;
    }
    return config;
}

TEveStraightLineSet* Geant4Track::GetEveDrawable() const {
    auto lineSet = new TEveStraightLineSet(TString::Format("ID%d | %s | Created by %s | Init KE: %0.2f keV",  //
                                                           fTrackID, fParticleName.Data(), fCreatorProcess.Data(), fInitialKineticEnergy));

    for (int i = 0; i < fHits.fN - 1; i++) {
        lineSet->AddLine(
            {static_cast<float>(fHits.fPosition[i].x()), static_cast<float>(fHits.fPosition[i].y()), static_cast<float>(fHits.fPosition[i].z())},  //
            {static_cast<float>(fHits.fPosition[i + 1].x()), static_cast<float>(fHits.fPosition[i + 1].y()),
             static_cast<float>(fHits.fPosition[i + 1].z())});

        const auto config = GetTrackVisualConfiguration(*this);
        lineSet->SetLineColor(config.fColor);
        lineSet->SetLineStyle(config.fLineStyle);
        lineSet->SetLineWidth(config.fLineWidth);
    }

    return lineSet;
}

TEvePointSet* Geant4Hits::GetEveDrawable() const { return nullptr; }

void Geant4Event::Draw() {
    size_t trackCounter = 0;
    size_t hitsCounter = 0;

    map<Int_t, TEveStraightLineSet*> linesSet;
    map<TString, TEvePointSet*> hitsPoints;
    map<TString, TEveElementList*> hitsType;

    auto trackList = new TEveElementList("Tracks");
    gEve->AddElement(trackList);
    auto hitsList = new TEveElementList("Hits");
    gEve->AddElement(hitsList);

    for (const auto& track : fTracks) {
        if (track.fInitialKineticEnergy < 1.0 || track.fTrackLength < 0.1) {
            continue;
        }

        auto line = track.GetEveDrawable();
        linesSet[track.fTrackID] = line;
        TEveElement* parentLine = trackList;
        if (linesSet.count(track.fParentID)) {
            parentLine = linesSet.at(track.fParentID);
        }
        gEve->AddElement(line, parentLine);
        trackCounter++;

        const auto& hits = track.fHits;
        for (int i = 0; i < hits.fN; i++) {
            //
            const auto& processName = hits.fProcessName[i];
            const auto& processType = hits.fProcessType[i];
            const auto& position = hits.fPosition[i];

            if (hitsType.count(processType) == 0) {
                hitsType[processType] = new TEveElementList(processType);
                gEve->AddElement(hitsType[processType], hitsList);
            }
            if (hitsPoints.count(processName) == 0) {
                hitsPoints[processName] = new TEvePointSet(processName);
                auto hitPoints = hitsPoints.at(processName);
                auto hitsVisualConfig = GetHitsVisualConfiguration(processName);
                hitPoints->SetMarkerColor(hitsVisualConfig.fColor);
                hitPoints->SetMarkerStyle(hitsVisualConfig.fMarkerStyle);
                hitPoints->SetMarkerSize(hitsVisualConfig.fMarkerSize);

                gEve->AddElement(hitPoints, hitsType[processType]);
            }
            hitsPoints.at(processName)->SetNextPoint(position.X(), position.Y(), position.Z());
            hitsCounter++;
        }
    }

    spdlog::info("Drawing {} tracks and {} hits", trackCounter, hitsCounter);
}