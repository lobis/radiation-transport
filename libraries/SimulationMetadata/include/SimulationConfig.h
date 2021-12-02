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
#include "SourceConfig.h"

class SimulationConfig : public SerializableConfig {
   public:
    void Deserialize(const YAML::Node&) override;
    YAML::Node Serialize() const override;

   public:
    std::string fConfigAbsolutePath;

    int fNumberOfEvents = 1;

    std::string fVerboseLevel = "info";
    std::string fRunManagerType = "serial";
    int fThreads = 2;
    int fSeed = 0;
    bool fFullChain = false;
    std::vector<std::string> fCommands;

    std::string fOutputFilename;

    DetectorConstructionConfig fDetectorConfig;
    PhysicsListConfig fPhysicsListConfig;
    SourceConfig fSourceConfig;

    //
    bool fSaveAllEvents = false;
    bool fSave = true;
    bool fInteractive = false;

   public:
    inline SimulationConfig() = default;
    SimulationConfig(const std::string&);

    void Print() const override;

    std::string GetOutputFileAbsolutePath() const;
};

#endif  // RADIATION_TRANSPORT_SIMULATIONCONFIG_H
