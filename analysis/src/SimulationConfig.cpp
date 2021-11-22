//
// Created by lobis on 11/22/2021.
//

#include "SimulationConfig.h"

#include <TSystem.h>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <iostream>

using namespace std;

SimulationConfig::SimulationConfig() : fSerialMode("serial"), fConfigFilename("") {}

SimulationConfig SimulationConfig::LoadFromFile(const string& filename) {
    if (gSystem->AccessPathName(filename.c_str(), kFileExists)) {
        spdlog::error("config file '{}' does not exist", filename);
        exit(1);
    }

    auto config = SimulationConfig();

    YAML::Node configNode = YAML::LoadFile(filename);

    if (configNode["runManagerType"]) {
        config.fSerialMode = configNode["runManagerType"].as<string>();
    }

    spdlog::info("Reading configuration '{}' contents:", filename);
    cout << configNode << endl;

    return config;
}
