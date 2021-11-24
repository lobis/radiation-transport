//
// Created by lobis on 11/22/2021.
//

#ifndef RADIATION_TRANSPORT_SIMULATIONCONFIG_H
#define RADIATION_TRANSPORT_SIMULATIONCONFIG_H

#include <yaml-cpp/yaml.h>

#include <string>
#include <vector>

class SimulationConfig {
   public:
    std::string fConfigAbsolutePath;

    std::string fRunManagerType = "serial";
    int fThreads = 2;
    int fSeed = 0;

    std::vector<std::string> fCommands;

    std::string fGeometryFilename;
    std::string GetGeometryAbsolutePath() const;

    inline std::string GetVerboseLevel() const { return fVerboseLevel; }
    void SetVerboseLevel(const std::string&);

   private:
    std::string fVerboseLevel = "info";

   public:
    inline SimulationConfig() = default;
    SimulationConfig(const std::string&);

    SimulationConfig static LoadFromFile(const std::string& filename);

    YAML::Node Serialize() const;
    void Print() const;
};

#endif  // RADIATION_TRANSPORT_SIMULATIONCONFIG_H
