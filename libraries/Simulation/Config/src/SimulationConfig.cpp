//
// Created by lobis on 11/22/2021.
//

#include "SimulationConfig.h"

#include <TSystem.h>
#include <spdlog/spdlog.h>

#include <iostream>

ClassImp(SimulationConfig);

using namespace std;

void SimulationConfig::Deserialize(const YAML::Node& node) {
    if (node["verboseLevel"]) {
        auto newVerboseLevel = node["verboseLevel"].as<string>();
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

    if (node["runManagerType"]) {
        fRunManagerType = node["runManagerType"].as<string>();
    }

    if (node["numberOfEvents"]) {
        fNumberOfEvents = node["numberOfEvents"].as<int>();
    }

    if (node["threads"]) {
        fThreads = node["threads"].as<int>();
    }

    if (node["seed"]) {
        fSeed = node["seed"].as<int>();
    }

    if (node["commands"]) {
        fCommands = node["commands"].as<vector<string>>();
    }

    if (node["output"]) {
        fOutputFilename = node["output"].as<string>();
    }

    if (node["fullChain"]) {
        fFullChain = node["fullChain"].as<bool>();
    }

    if (node["saveAllEvents"]) {
        fSaveAllEvents = node["saveAllEvents"].as<bool>();
    }

    if (node["interactive"]) {
        fInteractive = node["interactive"].as<bool>();
    }

    if (node["detector"]) {
        fDetectorConfig.Deserialize(node["detector"]);
        fDetectorConfig.fConfigAbsolutePath = fConfigAbsolutePath;
    }

    if (node["physics"]) {
        fPhysicsListConfig.Deserialize(node["physics"]);
    }

    if (node["source"]) {
        fSourceConfig.Deserialize(node["source"]);
    }
}

YAML::Node SimulationConfig::Serialize() const {
    YAML::Node node;

    node["verboseLevel"] = fVerboseLevel;
    node["runManagerType"] = fRunManagerType;
    node["numberOfEvents"] = fNumberOfEvents;
    node["threads"] = fThreads;
    node["seed"] = fSeed;
    node["commands"] = fCommands;
    node["output"] = fOutputFilename;
    node["fullChain"] = fFullChain;
    node["saveAllEvents"] = fSaveAllEvents;
    node["interactive"] = fInteractive;
    node["detector"] = fDetectorConfig.Serialize();
    node["physics"] = fPhysicsListConfig.Serialize();
    node["source"] = fSourceConfig.Serialize();

    return node;
}

SimulationConfig::SimulationConfig(const string& filename) {
    if (gSystem->AccessPathName(filename.c_str(), kFileExists)) {
        spdlog::error("SimulationConfig::SimulationConfig - config file '{}' does not exist", filename);
        exit(1);
    }

    fConfigAbsolutePath = GetAbsolutePath(filename);

    Deserialize(YAML::LoadFile(filename));
}

void SimulationConfig::Print() const {
    spdlog::info("Reading configuration '{}' contents:", fConfigAbsolutePath);
    cout << Serialize() << endl;
}

std::string SimulationConfig::GetOutputFileAbsolutePath() const { return GetAbsolutePath(fOutputFilename); }
