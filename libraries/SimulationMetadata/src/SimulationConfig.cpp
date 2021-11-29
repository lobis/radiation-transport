//
// Created by lobis on 11/22/2021.
//

#include "SimulationConfig.h"

#include <spdlog/spdlog.h>

#include <filesystem>
#include <iostream>

using namespace std;

namespace fs = std::filesystem;

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
    YAML::Node configNode;

    configNode["verboseLevel"] = fVerboseLevel;
    configNode["runManagerType"] = fRunManagerType;
    configNode["threads"] = fThreads;
    configNode["seed"] = fSeed;
    configNode["commands"] = fCommands;
    configNode["output"] = fOutputFilename;
    configNode["fullChain"] = fFullChain;

    configNode["detector"] = fDetectorConfig.Serialize();
    configNode["physics"] = fPhysicsListConfig.Serialize();
    configNode["source"] = fSourceConfig.Serialize();

    return configNode;
}

SimulationConfig::SimulationConfig(const string& filename) {
    if (!fs::exists(filename)) {
        spdlog::error("config file '{}' does not exist", filename);
        exit(1);
    }

    fConfigAbsolutePath = fs::absolute(filename);

    Deserialize(YAML::LoadFile(filename));
}

void SimulationConfig::Print() const {
    spdlog::info("Reading configuration '{}' contents:", fConfigAbsolutePath);
    cout << Serialize() << endl;
}

std::string SimulationConfig::GetOutputFileAbsolutePath() const {
    if (fOutputFilename.empty()) {
        return "";
    }
    if (fs::path(fOutputFilename).is_absolute()) {
        return fOutputFilename;
    }
    return fs::path(fConfigAbsolutePath).parent_path() / fOutputFilename;
}
