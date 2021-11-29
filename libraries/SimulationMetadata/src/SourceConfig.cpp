//
// Created by lobis on 11/25/2021.
//

#include "SourceConfig.h"

#include <spdlog/spdlog.h>

using namespace std;

namespace YAML {
template <>
struct convert<TVector3> {
    static Node encode(const TVector3& rhs) {
        Node node;
        node.push_back(rhs.x());
        node.push_back(rhs.x());
        node.push_back(rhs.x());
        return node;
    }

    static bool decode(const Node& node, TVector3& rhs) {
        if (!node.IsSequence() || node.size() != 3) {
            return false;
        }
        rhs = {node[0].as<double>(), node[1].as<double>(), node[2].as<double>()};
        return true;
    }
};
}  // namespace YAML

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

    // TODO: reduce code duplication
    if (generator["point"]) {
        fGeneratorType = "point";
        if (generator["point"]["position"]) {
            fGeneratorPosition = generator["point"]["position"].as<TVector3>();
        }
        if (generator["point"]["direction"]) {
            fGeneratorDirection = generator["point"]["direction"].as<TVector3>();
        }
    } else if (generator["plane"]) {
        fGeneratorType = "plane";
        if (generator["point"]["position"]) {
            fGeneratorPosition = generator["point"]["position"].as<TVector3>();
        }
        if (generator["point"]["direction"]) {
            fGeneratorDirection = generator["point"]["direction"].as<TVector3>();
        }
        fGeneratorSize = generator["point"]["size"].as<TVector3>();

    } else {
        spdlog::error("Invalid generator '{}'", generator.as<string>());
        exit(1);
    }
}

YAML::Node SourceConfig::Serialize() const {
    YAML::Node node;

    node["particle"] = YAML::Node();
    node["particle"]["name"] = fParticleName;
    node["particle"]["energy"] = fParticleEnergy;
    node["particle"]["excitedLevel"] = fParticleExcitedLevel;

    node["generator"] = YAML::Node();
    if (node["generator"].as<string>() == "point") {
        node["generator"]["position"] = fGeneratorPosition;
        node["generator"]["direction"] = fGeneratorDirection;
    } else if (node["generator"].as<string>() == "plane") {
        node["generator"]["position"] = fGeneratorPosition;
        node["generator"]["direction"] = fGeneratorDirection;
        node["generator"]["size"] = fGeneratorDirection;
    }

    return node;
}
