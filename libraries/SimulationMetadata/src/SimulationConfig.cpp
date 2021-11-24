//
// Created by lobis on 11/22/2021.
//

#include "SimulationConfig.h"

#include <TSystem.h>
#include <spdlog/spdlog.h>

#include <iostream>

using namespace std;

YAML::Node SimulationConfig::Serialize() const {
    YAML::Node configNode;

    configNode["verboseLevel"] = fVerboseLevel;
    configNode["runManagerType"] = fRunManagerType;
    configNode["threads"] = fThreads;
    configNode["seed"] = fSeed;
    configNode["commands"] = fCommands;

    return configNode;
}

SimulationConfig SimulationConfig::LoadFromFile(const string& filename) {
    if (gSystem->AccessPathName(filename.c_str(), kFileExists)) {
        spdlog::error("config file '{}' does not exist", filename);
        exit(1);
    }

    auto config = SimulationConfig();

    config.fConfigFilename = filename;

    YAML::Node configNode = YAML::LoadFile(filename);

    if (configNode["verboseLevel"]) {
        config.SetVerboseLevel(configNode["verboseLevel"].as<string>());
    }

    if (configNode["runManagerType"]) {
        config.fRunManagerType = configNode["runManagerType"].as<string>();
    }

    if (configNode["threads"]) {
        config.fThreads = configNode["threads"].as<int>();
    }

    if (configNode["seed"]) {
        config.fSeed = configNode["seed"].as<int>();
    }

    if (configNode["commands"]) {
        config.fCommands = configNode["commands"].as<vector<string>>();
    }

    return config;
}

void SimulationConfig::SetVerboseLevel(const string& newVerboseLevel) {
    if (newVerboseLevel == "debug") {
        spdlog::set_level(spdlog::level::debug);
    } else if (newVerboseLevel == "info") {
        spdlog::set_level(spdlog::level::info);
    } else if (newVerboseLevel == "warning") {
        spdlog::set_level(spdlog::level::warn);
    } else if (newVerboseLevel == "error") {
        spdlog::set_level(spdlog::level::err);
    } else {
        spdlog::error("Invalid 'verboseLevel' '{}'. Valid values are: 'debug', 'info', 'warning', 'error'", newVerboseLevel);
        exit(1);
    }
    fVerboseLevel = newVerboseLevel;
}

void SimulationConfig::Print() const {
    spdlog::info("Reading configuration '{}' contents:", fConfigFilename);
    cout << Serialize() << endl;
}
