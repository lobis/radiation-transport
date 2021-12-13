//
// Created by lobis on 23/11/2021.
//

#include "Geant4Hits.h"

#include <spdlog/spdlog.h>

ClassImp(Geant4Hits);

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

void Geant4Hits::Print() const {
    for (int i = 0; i < fN; i++) {
        spdlog::info("\t\tHit ID: {} - Energy: {:0.2f} keV - Process: {} ({}) - Volume: {} - Time: {:0.2f} ns - Length: {:0.2f} mm - Position: {} mm",
                     i, fEnergy[i], fProcessName[i], fProcessType[i],
                     fVolumeName[i] + (fVolumeNamePost[i].IsNull() ? "" : TString("->" + fVolumeNamePost[i]).Data()), fTimeGlobal[i], fLength[i],
                     fPosition[i]);
    }
}
