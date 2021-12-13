//
// Created by lobis on 23/11/2021.
//

#include "Geant4Track.h"

#include <spdlog/spdlog.h>

ClassImp(Geant4Track);

using namespace std;

namespace fmt {
template <>
struct formatter<TVector3> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.end(); }

    template <typename FormatContext>
    auto format(const TVector3& input, FormatContext& ctx) -> decltype(ctx.out()) {
        return format_to(ctx.out(), "({:0.2f}, {:0.2f}, {:0.2f})", input.x(), input.y(), input.z());
    }
};
}  // namespace fmt

void Geant4Track::Print() const {
    spdlog::info(
        "\tTrackID: {} - ParentID: {} - Particle Name: {} - Kinetic Energy: {:0.2f} keV - Created by: {} - Number of Secondaries: {} - Track Length: "
        "{:0.2f} mm",
        fTrackID, fParentID, fParticleName, fInitialKineticEnergy, fCreatorProcess, fNumberOfSecondaries, fTrackLength);
    fSteps.Print();
}

double Geant4Track::GetEnergyInVolume(const TString& volume) {
    if (fEnergyInVolumeMap.empty()) {
        InitializeEnergyInVolumeMap();
    }
    if (fEnergyInVolumeMap.count(volume.Data()) > 0) {
        return fEnergyInVolumeMap.at(volume.Data());
    }
    return 0;
}

void Geant4Track::InitializeEnergyInVolumeMap() {
    fEnergyInVolumeMap.clear();
    for (int i = 0; i < fSteps.fN; i++) {
        string volume = fSteps.fVolumeName[i].Data();
        double energy = fSteps.fEnergy[i];
        fEnergyInVolumeMap[volume] = fEnergyInVolumeMap[volume] + energy;
    }
}
