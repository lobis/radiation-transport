//
// Created by lobis on 11/22/2021.
//

#ifndef RADIATION_TRANSPORT_SIMULATIONCONFIG_H
#define RADIATION_TRANSPORT_SIMULATIONCONFIG_H

#include <yaml-cpp/yaml.h>

#include <filesystem>

#include "DetectorConstructionConfig.h"
#include "PhysicsListConfig.h"
#include "SerializableConfig.h"

class SimulationConfig : public SerializableConfig {
   public:
    void Deserialize(const YAML::Node&) override;
    YAML::Node Serialize() const override;

   public:
    std::string fConfigAbsolutePath;

    std::string fVerboseLevel = "info";
    std::string fRunManagerType = "serial";
    int fThreads = 2;
    int fSeed = 0;
    std::vector<std::string> fCommands;

    DetectorConstructionConfig fDetectorConfig;
    PhysicsListConfig fPhysicsListConfig;

   public:
    inline SimulationConfig() = default;
    SimulationConfig(const std::string&);

    void Print() const override;
};

#endif  // RADIATION_TRANSPORT_SIMULATIONCONFIG_H
