//
// Created by lobis on 06/12/2021.
//

#include "GasManager.h"

#include <spdlog/spdlog.h>

using namespace std;

using namespace Garfield;

ClassImp(GasManager);

GasManager::GasManager() : fGas(make_shared<MediumMagboltz>()) {
    const double pressureInBar = 1.0;
    const double temperature = 293.15;

    fGas->SetTemperature(temperature);
    SetGasPressureInBar(pressureInBar);
}

void GasManager::Print() const {
    spdlog::debug("GasGenerator::Print");
    spdlog::info("Gas Pressure: {:0.2f} bar - Gas Temperature: {:0.2f} K", fGas->GetPressure() / fUnitBar, fGas->GetTemperature());
}

void GasManager::SetGasPressureInBar(const double bar) {
    spdlog::debug("GasGenerator::SetGasPressureInBar - {:0.2f} bar", bar);
    fGas->SetPressure(bar * fUnitBar);
}

string GasManager::GetGasName() const {
    string gasName;
    for (int i = 0; i < fGas->GetNumberOfComponents(); i++) {
        string name = "";
        double fraction = 0;
        fGas->GetComponent(i, name, fraction);
        spdlog::info("{} {} {}", i, name, fraction);
        gasName += TString::Format("%s-%0.2f", name.c_str(), fraction) + "%_";
    }

    gasName += TString::Format("P=%0.2fbar", fGas->GetPressure() / fUnitBar);
    gasName += "_";
    gasName += TString::Format("T=%0.2fC", fGas->GetTemperature() - 273.15);
    gasName += ".gas";

    return gasName;
}

string GasManager::GenerateGas(const string& directory) {
    string prefix = "";
    if (directory.empty()) {
        prefix = string(get_current_dir_name());
    } else {
        prefix = directory;
    }

    string gasFilename = prefix + "/" + GetGasName();

    spdlog::info("GasGenerator::GenerateGas - Gas will be saved in '{}'", gasFilename);

    // Set the field range to be covered by the gas table.
    const size_t numberOfNodesE = 20;
    const double eMin = 100.;
    const double eMax = 100000.;

    constexpr bool useLogarithmicSpacing = true;
    fGas->SetFieldGrid(eMin, eMax, numberOfNodesE, useLogarithmicSpacing);

    const int numberOfCollisions = 10;

    fGas->GenerateGasTable(numberOfCollisions);
    fGas->WriteGasFile(gasFilename);

    return gasFilename;
}

void GasManager::ReadGasFromFile(const string& gasFilename) { fGas->LoadGasFile(gasFilename); }

double GasManager::GetDriftVelocity(double E) {
    double vx, vy, vz;
    fGas->ElectronVelocity(
        //
        0, 0, -E,   // Electric field
        0, 0, 0,    // Magnetic field (zero)
        vx, vy, vz  //
    );
    return vz * 1E3;  // result in cm/us
}

double GasManager::GetDiffusionCoefficientLongitudinal(double E) {
    double diffusionLongitudinal, diffusionTransversal;
    fGas->ElectronDiffusion(
        //
        0, 0, -E,                                    // Electric field
        0, 0, 0,                                     // Magnetic field (zero)
        diffusionLongitudinal, diffusionTransversal  //
    );
    return diffusionLongitudinal;  // result in cm^(1/2)
}

double GasManager::GetDiffusionCoefficientTransversal(double E) {
    double diffusionLongitudinal, diffusionTransversal;
    fGas->ElectronDiffusion(
        //
        0, 0, -E,                                    // Electric field
        0, 0, 0,                                     // Magnetic field (zero)
        diffusionLongitudinal, diffusionTransversal  //
    );
    return diffusionTransversal;  // result in cm^(1/2)
}
