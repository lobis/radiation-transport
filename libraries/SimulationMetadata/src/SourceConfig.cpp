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

    if (particle["name"]) {
        fParticleName = particle["name"].as<string>();
    }

    if (particle["excitedLevel"]) {
        fParticleExcitedLevel = particle["excitedLevel"].as<double>();
    }

    DeserializeGenerator(node);
    DeserializeEnergy(node);
    DeserializeAngular(node);
}

YAML::Node SourceConfig::Serialize() const {
    YAML::Node node;

    node["particle"] = YAML::Node();
    node["particle"]["name"] = fParticleName;
    node["particle"]["excitedLevel"] = fParticleExcitedLevel;

    node["generator"] = SerializeGenerator();
    node["energy"] = SerializeEnergy();
    node["angular"] = SerializeAngular();

    return node;
}

void SourceConfig::DeserializeGenerator(const YAML::Node& node) {
    if (!node["generator"]) {
        return;
    }

    auto generator = node["generator"];
    // check one and only one generator type is present

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

void SourceConfig::DeserializeEnergy(const YAML::Node& node) {
    if (!node["energy"]) {
        return;
    }

    auto energy = node["energy"];

    {
        size_t count = 0;
        for (const auto& type : fEnergyDistributionTypesAvailable) {
            if (energy[type]) {
                count += 1;
                fEnergyDistributionType = type;
            }
        }
        if (count != 1) {
            spdlog::error("SourceConfig::Deserialize - zero or more than one types of energy distribution present");
            exit(1);
        }
    }

    if (energy[fEnergyDistributionType]["unit"]) {
        fEnergyDistributionUnits = energy[fEnergyDistributionType]["unit"].as<string>();
    }
    if (energy[fEnergyDistributionType]["min"]) {
        fEnergyDistributionLimitMin = energy[fEnergyDistributionType]["min"].as<double>();
    }
    if (energy[fEnergyDistributionType]["max"]) {
        fEnergyDistributionLimitMax = energy[fEnergyDistributionType]["max"].as<double>();
    }
    if (fEnergyDistributionType == "mono") {
        if (energy[fEnergyDistributionType]["value"]) {
            fEnergyDistributionMonoValue = energy[fEnergyDistributionType]["value"].as<double>();
        }
    } else {
        spdlog::error("Not implemented yet");
        exit(1);
    }
}

void SourceConfig::DeserializeAngular(const YAML::Node& node) {
    if (!node["angular"]) {
        return;
    }

    auto angular = node["angular"];

    {
        size_t count = 0;
        for (const auto& type : fAngularDistributionTypesAvailable) {
            if (angular[type]) {
                count += 1;
                fAngularDistributionType = type;
            }
        }
        if (count != 1) {
            spdlog::error("SourceConfig::Deserialize - zero or more than one types of energy distribution present");
            exit(1);
        }
    }
}

YAML::Node SourceConfig::SerializeGenerator() const {
    YAML::Node node;

    node["type"] = fGeneratorType;

    if (fGeneratorType == "point" || fGeneratorType == "plane" || fGeneratorType == "disk") {
        node["position"] = fGeneratorPosition;
    }
    if (fGeneratorType == "plane" || fGeneratorType == "disk") {
        node["direction"] = fGeneratorDirection;
    }
    if (fGeneratorType == "plane") {
        node["size"] = fGeneratorSize;
    }
    if (fGeneratorType == "disk") {
        node["diameter"] = fGeneratorDiameter;
    }

    return node;
}

YAML::Node SourceConfig::SerializeEnergy() const {
    YAML::Node node;

    node["type"] = fEnergyDistributionType;

    node["unit"] = fEnergyDistributionUnits;

    node["min"] = fEnergyDistributionLimitMin;
    node["max"] = fEnergyDistributionLimitMax;

    if (fEnergyDistributionType == "mono") {
        node["value"] = fGeneratorPosition;
    }

    return node;
}

YAML::Node SourceConfig::SerializeAngular() const {
    YAML::Node node;

    node["type"] = fAngularDistributionType;

    node["thetaMin"] = fAngularDistributionLimitThetaMin;
    node["thetaMax"] = fAngularDistributionLimitThetaMax;

    node["phiMin"] = fAngularDistributionLimitPhiMin;
    node["phiMax"] = fAngularDistributionLimitPhiMax;

    if (fAngularDistributionType == "flux") {
        node["direction"] = fAngularDistributionDirection;
    }

    return node;
}
