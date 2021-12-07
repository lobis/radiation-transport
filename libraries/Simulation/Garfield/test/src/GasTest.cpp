//
// Created by lobis on 30/11/2021.
//

#include <GasManager.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <Garfield/FundamentalConstants.hh>
#include <filesystem>

using namespace std;

namespace fs = std::filesystem;

using namespace Garfield;

#define REPOSITORY_ROOT string(std::filesystem::path(__FILE__).parent_path().parent_path().parent_path().parent_path().parent_path().parent_path())
#define SIMULATION_EXAMPLES string(REPOSITORY_ROOT + "/applications/simulation/examples/")
#define FILES_PATH string(std::filesystem::path(__FILE__).parent_path().parent_path()) + "/files/"

TEST(GasManager, GenerateSetup) {
    GasManager gasManager;

    auto gas = gasManager.fGas;

    gasManager.SetGasPressureInBar(1.4);

    gas->SetComposition("Ar", 98.0, "isobutane", 2.0);

    gasManager.Print();

    gas->PrintGas();

    // gasManager.GenerateGas();
}

TEST(GasManager, GasParameters) {
    GasManager gasManager;
    gasManager.ReadGasFromFile(FILES_PATH + "test.gas");
    gasManager.Print();

    auto v = gasManager.GetDriftVelocity(100);

    spdlog::info("velocity: {:0.2f} cm/ns", v);
}