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

    // check one and only one generator type is present
    {
        size_t count = 0;
        for (const auto& type : fGeneratorTypesAvailable) {
            if (generator[type]) {
                count += 1;
                fGeneratorType = type;
            }
        }
        if (count != 1) {
            spdlog::error("SourceConfig::Deserialize - zero or more than one types of generator present");
            exit(1);
        }
    }

    if (fGeneratorType == "point" || fGeneratorType == "plane" || fGeneratorType == "disk") {
        if (generator[fGeneratorType]["position"]) {
            fGeneratorPosition = generator[fGeneratorType]["position"].as<TVector3>();
        }
    }

    if (fGeneratorType == "plane" || fGeneratorType == "disk") {
        if (generator[fGeneratorType]["direction"]) {
            fGeneratorDirection = generator[fGeneratorType]["direction"].as<TVector3>();
            if (fGeneratorDirection == TVector3({0, 0, 0})) {
                spdlog::error("SourceConfig::Deserialize - direction cannot be the zero vector");
                exit(1);
            }
        }
    }
    if (fGeneratorType == "plane") {
        fGeneratorSize = generator["point"]["size"].as<TVector3>();
    }
    if (fGeneratorType == "disk") {
        fGeneratorDiameter = generator["point"]["diameter"].as<double>();
    }
}

YAML::Node SourceConfig::Serialize() const {
    YAML::Node node;

    node["particle"] = YAML::Node();
    node["particle"]["name"] = fParticleName;
    node["particle"]["energy"] = fParticleEnergy;
    node["particle"]["excitedLevel"] = fParticleExcitedLevel;

    node["generator"] = YAML::Node();
    if (fGeneratorType == "point" || fGeneratorType == "plane" || fGeneratorType == "disk") {
        node["generator"]["position"] = fGeneratorPosition;
    }
    if (fGeneratorType == "plane" || fGeneratorType == "disk") {
        node["generator"]["direction"] = fGeneratorDirection;
    }
    if (fGeneratorType == "plane") {
        node["generator"]["size"] = fGeneratorDirection;
    }
    if (fGeneratorType == "disk") {
        node["generator"]["diameter"] = fGeneratorDiameter;
    }

    return node;
}
