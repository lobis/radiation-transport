//
// Created by lobis on 30/11/2021.
//

#include <GasGenerator.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <Garfield/FundamentalConstants.hh>
#include <filesystem>

using namespace std;

namespace fs = std::filesystem;

using namespace Garfield;

#define FILES_PATH string(std::filesystem::path(__FILE__).parent_path().parent_path()) + "/files/"

TEST(GasGenerator, Generate) {
    // Based on Garfield example (https://gitlab.cern.ch/garfield/garfieldpp/-/blob/master/Examples/GasFile/generate.C)

    spdlog::info("GasGenerator::Generate");

    GasGenerator gasGenerator;

    auto gas = gasGenerator.fGas;

    const double pressure = 1.4 * AtmosphericPressure;

    gas->SetComposition("Ar", 93., "CO2", 7.);
    gas->SetPressure(pressure);

    gasGenerator.Print();

    return;
    // Set the field range to be covered by the gas table.
    const size_t nE = 20;
    const double emin = 100.;
    const double emax = 100000.;

    constexpr bool useLog = true;  // Flag to request logarithmic spacing.
    gas->SetFieldGrid(emin, emax, nE, useLog);

    const int ncoll = 10;
    // Run Magboltz to generate the gas table.
    gas->GenerateGasTable(ncoll);
    // Save the table.
    gas->WriteGasFile("ar_93_co2_7.gas");

    spdlog::info("DONE!");
}