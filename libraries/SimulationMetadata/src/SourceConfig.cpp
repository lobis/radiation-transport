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
        node.push_back(rhs.y());
        node.push_back(rhs.z());
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
    if (!node["particle"]) {
        spdlog::warn("SourceConfig::Deserialize - 'particle' not set, using default");
    }

    if (node["particle"]) {
        auto particle = node["particle"];

        fParticleName = particle["name"].as<string>();

        if (particle["excitedLevel"]) {
            fParticleExcitedLevel = particle["excitedLevel"].as<double>();
        }
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

YAML::Node SourceConfig::SerializeGenerator() const {
    YAML::Node node;

    node["unit"] = fPositionDistributionUnit;

    node[fPositionDistributionType] = YAML::Node();

    if (fPositionDistributionType == "point" || fPositionDistributionType == "rectangle" || fPositionDistributionType == "square" ||
        fPositionDistributionType == "disk") {
        node[fPositionDistributionType]["position"] = fPositionDistributionCenter;
    }
    if (fPositionDistributionType == "rectangle" || fPositionDistributionType == "square" || fPositionDistributionType == "disk") {
        node[fPositionDistributionType]["direction"] = fPositionDistributionOrientation;
    }
    if (fPositionDistributionType == "rectangle") {
        node[fPositionDistributionType]["sideLong"] = fPositionDistributionRectangleSideLong;
        node[fPositionDistributionType]["sideShort"] = fPositionDistributionRectangleSideShort;
    }
    if (fPositionDistributionType == "square") {
        node[fPositionDistributionType]["side"] = fPositionDistributionSquareSide;
    }
    if (fPositionDistributionType == "disk") {
        node[fPositionDistributionType]["diameter"] = fPositionDistributionDiameter;
    }

    return node;
}

void SourceConfig::DeserializeGenerator(const YAML::Node& node) {
    if (!node["generator"]) {
        spdlog::warn("SourceConfig::Deserialize - 'generator' not set, using default");
        return;
    }

    auto generator = node["generator"];

    if (generator["unit"]) {
        fPositionDistributionUnit = generator["unit"].as<string>();
    }

    // check one and only one generator type is present
    size_t count = 0;
    for (const auto& type : fGeneratorTypesAvailable) {
        if (generator[type]) {
            count += 1;
            fPositionDistributionType = type;
        }
    }

    if (count != 1) {
        spdlog::error("SourceConfig::Deserialize - zero or more than one types of generator present");
        exit(1);
    }

    if (fPositionDistributionType == "point" || fPositionDistributionType == "rectangle" || fPositionDistributionType == "square" ||
        fPositionDistributionType == "disk") {
        if (generator[fPositionDistributionType]["position"]) {
            fPositionDistributionCenter = generator[fPositionDistributionType]["position"].as<TVector3>();
        }
    }

    if (fPositionDistributionType == "rectangle" || fPositionDistributionType == "square" || fPositionDistributionType == "disk") {
        if (generator[fPositionDistributionType]["direction"]) {
            fPositionDistributionOrientation = generator[fPositionDistributionType]["direction"].as<TVector3>();
            if (fPositionDistributionOrientation == TVector3({0, 0, 0})) {
                spdlog::error("SourceConfig::Deserialize - direction cannot be the zero vector");
                exit(1);
            }
        }
    }
    if (fPositionDistributionType == "square") {
        fPositionDistributionSquareSide = generator[fPositionDistributionType]["side"].as<double>();
    }
    if (fPositionDistributionType == "rectangle") {
        fPositionDistributionRectangleSideLong = generator[fPositionDistributionType]["sideLong"].as<double>();
        fPositionDistributionRectangleSideShort = generator[fPositionDistributionType]["sideShort"].as<double>();
    }
    if (fPositionDistributionType == "disk") {
        fPositionDistributionDiameter = generator[fPositionDistributionType]["diameter"].as<double>();
    }
}

YAML::Node SourceConfig::SerializeEnergy() const {
    YAML::Node node;

    node["unit"] = fEnergyDistributionUnit;

    node["min"] = fEnergyDistributionLimitMin;
    node["max"] = fEnergyDistributionLimitMax;

    node[fEnergyDistributionType] = YAML::Node();

    if (fEnergyDistributionType == "mono") {
        node[fEnergyDistributionType]["value"] = fEnergyDistributionMonoValue;
    }

    return node;
}

void SourceConfig::DeserializeEnergy(const YAML::Node& node) {
    if (!node["energy"]) {
        spdlog::warn("SourceConfig::Deserialize - 'energy' not set, using default");
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

    if (energy["unit"]) {
        fEnergyDistributionUnit = energy["unit"].as<string>();
    }
    if (energy["min"]) {
        fEnergyDistributionLimitMin = energy["min"].as<double>();
    }
    if (energy["max"]) {
        fEnergyDistributionLimitMax = energy["max"].as<double>();
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

YAML::Node SourceConfig::SerializeAngular() const {
    YAML::Node node;

    node["thetaMin"] = fAngularDistributionLimitThetaMin;
    node["thetaMax"] = fAngularDistributionLimitThetaMax;

    node["phiMin"] = fAngularDistributionLimitPhiMin;
    node["phiMax"] = fAngularDistributionLimitPhiMax;

    node[fAngularDistributionType] = YAML::Node();

    if (fAngularDistributionType == "flux") {
        node[fAngularDistributionType]["direction"] = fAngularDistributionDirection;
    }

    return node;
}

void SourceConfig::DeserializeAngular(const YAML::Node& node) {
    if (!node["angular"]) {
        spdlog::warn("SourceConfig::Deserialize - 'angular' not set, using default");
        return;
    }

    auto angular = node["angular"];

    size_t count = 0;
    for (const auto& type : fAngularDistributionTypesAvailable) {
        if (angular[type]) {
            count += 1;
            fAngularDistributionType = type;
        }
    }
    if (count != 1) {
        spdlog::error("SourceConfig::Deserialize - zero or more than one types of angular distribution present");
        exit(1);
    }

    if (angular["thetaMin"]) {
        fAngularDistributionLimitThetaMin = angular["thetaMin"].as<double>();
    }
    if (angular["thetaMax"]) {
        fAngularDistributionLimitThetaMin = angular["thetaMax"].as<double>();
    }
    if (angular["phiMin"]) {
        fAngularDistributionLimitThetaMin = angular["phiMin"].as<double>();
    }
    if (angular["phiMax"]) {
        fAngularDistributionLimitThetaMin = angular["phiMax"].as<double>();
    }

    if (fAngularDistributionType == "flux") {
        if (!angular[fAngularDistributionType]["direction"]) {
            spdlog::error("SourceConfig::DeserializeAngular - angular distribution: '{}' but direction not set", fAngularDistributionType);
            exit(1);
        }
        fAngularDistributionDirection = angular[fAngularDistributionType]["direction"].as<TVector3>();
    }
}