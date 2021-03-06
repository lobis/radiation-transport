//
// Created by lobis on 25/11/2021.
//

#ifndef RADIATION_TRANSPORT_PHYSICSLISTCONFIG_H
#define RADIATION_TRANSPORT_PHYSICSLISTCONFIG_H

#include "SerializableConfig.h"

class PhysicsListConfig : public SerializableConfig {
    ClassDef(PhysicsListConfig, 1);

   public:
    inline PhysicsListConfig() = default;

    void Deserialize(const YAML::Node&) override;
    YAML::Node Serialize() const override;

   public:  // Attributes must follow schema
    int fVerbosity = 0;
    double fLengthProductionCutsGlobal = 0.5 /* mm */;

    double fEnergyProductionCutsMin = 0.1 /* keV */;
    double fEnergyProductionCutsMax = 100 * 1E6 /* keV */;

    std::vector<std::string> fPhysicsLists;
};

#endif  // RADIATION_TRANSPORT_PHYSICSLISTCONFIG_H
