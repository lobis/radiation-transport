//
// Created by lobis on 22/11/2021.
//

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <G4RunManagerFactory.hh>
#include <G4UIExecutive.hh>
#include <G4UImanager.hh>
#include <G4VisExecutive.hh>

#include "SimulationConfig.h"

int main() {
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%T][%^%l%$][thread %t]: %v");

    spdlog::info("This is the simulation application!");

    auto config = SimulationConfig::LoadFromFile("../analysis/test/files/simulation.yaml");

    return 0;
}