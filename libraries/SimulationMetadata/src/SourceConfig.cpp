//
// Created by lobis on 11/25/2021.
//

#include "SourceConfig.h"

#include <spdlog/spdlog.h>

using namespace std;

void SourceConfig::Deserialize(const YAML::Node& node) {
    if (!node["particle"] || !node["generator"]) {
        spdlog::error("Bad parameters in particle source. You need to set 'particle' and 'generator'");
    }
    auto particle = node["particle"];
    auto generator = node["generator"];

    if (particle["name"]) {
        fParticleName = particle["name"].as<string>();
    }

    if (particle["energy"]) {
        fParticleEnergy = particle["energy"].as<double>();
    }

    if (particle["excitedLevel"]) {
        fParticleExcitedLevel = particle["excitedLevel"].as<double>();
    }

    if (generator["point"]) {
        fGeneratorType = "point";
    } else if (generator["plane"]) {
        fGeneratorType = "plane";
    } else {
        spdlog::error("Invalid generator '{}'", generator.as<string>());
        exit(1);
    }
}

YAML::Node SourceConfig::Serialize() const { return YAML::Node(); }
