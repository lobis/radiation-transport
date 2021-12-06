//
// Created by lobis on 06/12/2021.
//

#include "GasGenerator.h"

#include <spdlog/spdlog.h>

using namespace std;

using namespace Garfield;

ClassImp(GasGenerator);

GasGenerator::GasGenerator() : fGas(make_shared<MediumMagboltz>()) {
    const double pressureInBar = 1.0;
    const double temperature = 293.15;

    fGas->SetTemperature(temperature);
    SetGasPressureInBar(pressureInBar);
}

void GasGenerator::Print() const {
    spdlog::info("GasGenerator::Print");
    spdlog::info("Gas Pressure: {:0.2f} bar - Gas Temperature: {:0.2f} K", fGas->GetPressure() / fUnitBar, fGas->GetTemperature());
}

void GasGenerator::SetGasPressureInBar(const double bar) {
    spdlog::info("GasGenerator::SetGasPressureInBar - {:0.2f} bar", bar);
    fGas->SetPressure(bar * fUnitBar);
}
