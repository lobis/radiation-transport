//
// Created by lobis on 25/11/2021.
//

#include "PhysicsListConfig.h"

#include <iostream>

ClassImp(PhysicsListConfig);

using namespace std;

void PhysicsListConfig::Deserialize(const YAML::Node& node) {
    if (node["verbosity"]) {
        fVerbosity = node["verbosity"].as<int>();
    }

    if (node["productionCutsLengthGlobal"]) {
        fLengthProductionCutsGlobal = node["productionCutsLengthGlobal"].as<double>();
    }

    if (node["productionCutsEnergyMin"]) {
        fEnergyProductionCutsMin = node["productionCutsEnergyMin"].as<double>();
    }

    if (node["productionCutsEnergyMax"]) {
        fEnergyProductionCutsMax = node["productionCutsEnergyMax"].as<double>();
    }

    if (node["physicsLists"]) {
        auto physicsLists = node["physicsLists"].as<vector<string>>();
        for (auto& physicsList : physicsLists) {
            for (auto& existingPhysicsLists : fPhysicsLists) {
                if (physicsList == existingPhysicsLists) {
                    continue;
                }
            }
            fPhysicsLists.push_back(physicsList);
        }
    }
}

YAML::Node PhysicsListConfig::Serialize() const {
    YAML::Node node;

    node["verbosity"] = fVerbosity;
    node["productionCutsLengthGlobal"] = fLengthProductionCutsGlobal;
    node["productionCutsEnergyMin"] = fEnergyProductionCutsMin;
    node["productionCutsEnergyMax"] = fEnergyProductionCutsMax;
    node["physicsLists"] = fPhysicsLists;

    return node;
}
