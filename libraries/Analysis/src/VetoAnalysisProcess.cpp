//
// Created by lobis on 3/7/2022.
//

#include "VetoAnalysisProcess.h"

#include <assert.h>
#include <spdlog/spdlog.h>

#include "VetoEvent.h"

ClassImp(VetoAnalysisProcess);

using namespace std;

VetoEvent VetoAnalysisProcess::SimulationEventToVetoEvent(const Geant4Event& input) {
    VetoEvent output;

    output.fRunID = input.fRunID;
    output.fEventID = input.fEventID;
    output.fSubEventID = input.fSubEventID;

    const auto sensitiveVolumes = fGeant4GeometryInfo.GetAllPhysicalVolumesMatchingExpression("gasVolume");
    assert(sensitiveVolumes.size() == 1);
    const auto sensitiveVolume = fGeant4GeometryInfo.GetAlternativeNameFromGeant4PhysicalName(sensitiveVolumes[0]);

    // spdlog::info("Sensitive Volume: {}", sensitiveVolume);

    for (const auto& track : input.fTracks) {
        const auto hits = track.fHits;
        for (int i = 0; i < hits.fN; i++) {
            const auto energy = hits.fEnergy[i];
            const auto volumeName = hits.fVolumeName[i];

            if (energy <= 0 || (!IsVetoVolume(volumeName) && volumeName != sensitiveVolume)) {
                continue;
            }

            double distanceToDetector = 0;
            if (IsVetoVolume(volumeName)) {
                const auto position = hits.fPosition[i];
                distanceToDetector = fScintillatorBoundaryPosition[volumeName] * fScintillatorBoundaryDirection[volumeName] -
                                     position * fScintillatorBoundaryDirection[volumeName];
            }

            output.fN += 1;
            output.fPosition.emplace_back(hits.fPosition[i]);
            output.fTimeGlobal.emplace_back(hits.fTimeGlobal[i]);
            output.fEnergy.emplace_back(hits.fEnergy[i]);
            output.fVolumeName.emplace_back(hits.fVolumeName[i]);
            output.fParticleName.emplace_back(track.fParticleName);
            output.fParticleType.emplace_back(track.fParticleType);
            output.fProcessName.emplace_back(hits.fProcessName[i]);
            output.fProcessType.emplace_back(hits.fProcessType[i]);
            output.fDistanceToDetector.emplace_back(distanceToDetector);
        }
    }

    return output;
}

void VetoAnalysisProcess::SetVetoInformation(const TString& scintillatorNameExpression, const TString& scintillatorDetectorsNameExpression) {
    // this assumes scintillators and "detectors" (light guides etc.) are stored in the correct order, they match 1 to 1

    fScintillatorNames.clear();
    for (const auto& volume : fGeant4GeometryInfo.GetAllPhysicalVolumesMatchingExpression(scintillatorNameExpression)) {
        fScintillatorNames.emplace_back(fGeant4GeometryInfo.GetAlternativeNameFromGeant4PhysicalName(volume));
    }

    fScintillatorDetectorNames.clear();
    for (const auto& volume : fGeant4GeometryInfo.GetAllPhysicalVolumesMatchingExpression(scintillatorDetectorsNameExpression)) {
        fScintillatorDetectorNames.emplace_back(fGeant4GeometryInfo.GetAlternativeNameFromGeant4PhysicalName(volume));
    }

    for (int i = 0; i < fScintillatorNames.size(); i++) {
        const auto scintillatorName = fScintillatorNames[i];
        const auto positionScintillator = fGeant4GeometryInfo.GetPositionInWorld(scintillatorName);

        const auto scintillatorDetectorName = fScintillatorDetectorNames[i];
        const auto positionScintillatorDetector = fGeant4GeometryInfo.GetPositionInWorld(scintillatorDetectorName);

        const auto distance = positionScintillatorDetector - positionScintillator;
        const auto direction = distance.Unit();

        fScintillatorBoundaryDirection[scintillatorName] = direction;
        fScintillatorBoundaryPosition[scintillatorName] = positionScintillatorDetector - direction * 0;

        // spdlog::info("distance {}: {} {} {}", scintillatorName, distance.X(), distance.Y(), distance.Z());
    }

    assert(fScintillatorNames.size() == fScintillatorDetectorNames.size());
    assert(fScintillatorNames.size() > 0);

    spdlog::info("there are {} scintillator volumes", fScintillatorNames.size());
}
