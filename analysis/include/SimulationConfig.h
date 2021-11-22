//
// Created by lobis on 11/22/2021.
//

#ifndef RADIATION_TRANSPORT_SIMULATIONCONFIG_H
#define RADIATION_TRANSPORT_SIMULATIONCONFIG_H

#include <string>

class SimulationConfig {
   public:
    std::string fSerialMode;
    std::string fConfigFilename;

   private:
    SimulationConfig();

   public:
    SimulationConfig static LoadFromFile(const std::string& filename);

    std::string Serialize() const;
};

#endif  // RADIATION_TRANSPORT_SIMULATIONCONFIG_H
