//
// Created by lobis on 3/7/2022.
//

#ifndef RADIATION_TRANSPORT_VETOANALYSISPROCESS_H
#define RADIATION_TRANSPORT_VETOANALYSISPROCESS_H

#include "Geant4Event.h"
#include "VetoEvent.h"

class VetoAnalysisProcess {
    ClassDef(VetoAnalysisProcess, 1);

   private:
    Geant4GeometryInfo fGeant4GeometryInfo;

    std::vector<TString> fScintillatorNames;
    std::vector<TString> fScintillatorDetectorNames;

    std::map<TString, TVector3> fScintillatorBoundaryPosition;
    std::map<TString, TVector3> fScintillatorBoundaryDirection;

    TString fDetectorName;
    TString fDetectorReadoutName;

   public:
    inline VetoAnalysisProcess() = default;

    inline void SetGeometryInfo(const Geant4GeometryInfo& geometryInfo) { fGeant4GeometryInfo = geometryInfo; }

    void SetVetoInformation(const TString& scintillatorNameExpression = "scintillatorVolume",
                            const TString& scintillatorDetectorsNameExpression = "scintillatorLightGuideVolume");
    void SetDetectorInformation(const TString& scintillatorNameExpression, const TString& scintillatorDetectorsNameExpression);

    inline bool IsVetoVolume(const TString& volume) const {
        for (const auto& scintillatorName : fScintillatorNames) {
            if (scintillatorName == volume) {
                return true;
            }
        }
        return false;
    }

    VetoEvent SimulationEventToVetoEvent(const Geant4Event& input);
};

#endif  // RADIATION_TRANSPORT_VETOANALYSISPROCESS_H
