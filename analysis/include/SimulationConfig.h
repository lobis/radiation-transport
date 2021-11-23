//
// Created by lobis on 11/22/2021.
//

#ifndef RADIATION_TRANSPORT_SIMULATIONCONFIG_H
#define RADIATION_TRANSPORT_SIMULATIONCONFIG_H

#include <string>
#include <vector>

class SimulationConfig {
   public:
    std::string fConfigFilename;

    std::string fRunManagerType = "serial";
    int fThreads = 2;
    int fSeed = 0;

    std::vector<std::string> fCommands;

    inline std::string GetVerboseLevel() const { return fVerboseLevel; }
    void SetVerboseLevel(const std::string&);

   private:
    std::string fVerboseLevel = "info";

    SimulationConfig();

   public:
    SimulationConfig static LoadFromFile(const std::string& filename);

    std::string Serialize() const;
};

#endif  // RADIATION_TRANSPORT_SIMULATIONCONFIG_H
