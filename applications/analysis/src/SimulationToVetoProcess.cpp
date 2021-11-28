//
// Created by lobis on 28/11/2021.
//

#include "SimulationToVetoProcess.h"

#include <TSystem.h>
#include <spdlog/spdlog.h>
// ClassImp(SimulationToVetoProcess);
#include <TCanvas.h>
#include <TH1D.h>

#include <exception>

using namespace std;

VetoEvent SimulationToVetoProcess::Process(const DataEvent& input) {
    VetoEvent output;
    SimulationGeometryInfo geometryInfo = *input.fSimulationGeometryInfo;

    if (fVetoNameToIDMap.empty()) {
        AddGeometryInfo(geometryInfo);
    }

    output.fRunID = input.fRunID;
    output.fEventID = input.fEventID;
    output.fSubEventID = input.fSubEventID;

    if (fVetoVolumes.empty()) {
        for (const TString& volume : geometryInfo.fGdmlNewPhysicalNames) {
            if (!volume.Contains("scintillatorVolume")) {
                continue;
            }
            fVetoVolumes.insert(volume);
        }
    }

    output.fNumberOfVetoes = fVetoVolumes.size();

    for (const auto& track : input.fTracks) {
        for (int i = 0; i < track.fSteps.fN; i++) {
            const auto energy = track.fSteps.fEnergy[i];
            if (energy <= 0) {
                continue;
            }
            const auto volume = track.fSteps.fVolumeName[i];
            if (fVetoVolumes.count(volume) <= 0) {
                continue;
            }
            if (!output.IsVetoAdded(volume)) {
                output.fVetoName.push_back(volume);
                // spdlog::info("{}", volume);
                output.fVetoID.push_back(fVetoNameToIDMap.at(volume));
                output.fVetoRawEnergy.emplace_back(0);

                output.fVetoHitsRawEnergy.push_back({});
                output.fVetoHitsTime.push_back({});
                output.fVetoHitsPosition.push_back({});
            }
            for (int j = 0; j < output.fVetoName.size(); j++) {
                // spdlog::warn("{}: {}", output.fVetoName[j], volume);
                if (output.fVetoName[j].EqualTo(volume)) {
                    output.fVetoRawEnergy[j] += energy;

                    output.fVetoHitsRawEnergy[j].emplace_back(energy);
                    output.fVetoHitsTime[j].emplace_back(track.fSteps.fTimeGlobal[i]);
                    output.fVetoHitsPosition[j].emplace_back(track.fSteps.fPosition[i]);
                }
            }
        }
    }
    /*
    for (int i = 0; i < output.fVetoName.size(); i++) {
        spdlog::info("\tVeto: {}, Energy: {:0.002f} keV, Hits: {}", output.fVetoName[i], output.fVetoRawEnergy[i],
                     output.fVetoHitsRawEnergy[i].size() 0);
    }
    */
    return output;
}

void SimulationToVetoProcess::AddGeometryInfo(const SimulationGeometryInfo& geometryInfo) {
    fVetoVolumes.clear();
    for (int i = 0; i < geometryInfo.fGdmlNewPhysicalNames.size(); i++) {
        const TString volume = geometryInfo.fGdmlNewPhysicalNames[i];
        if (volume.Contains("scintillatorVolume")) {
            fVetoVolumes.insert(volume);
            if (fVetoNameToIDMap.count(volume) > 0) {
                spdlog::error("volume already added");
                exit(1);
            }
            fVetoNameToIDMap[volume] = i;
        }
    }

    /*
    for (const auto& [name, id] : fVetoNameToIDMap) {
        spdlog::info("{}: {}", name, id);
    }
     */
}
