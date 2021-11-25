//
// Created by lobis on 24/11/2021.
//

#ifndef RADIATION_TRANSPORT_APPLICATION_H
#define RADIATION_TRANSPORT_APPLICATION_H

#include "SimulationConfig.h"

class Application {
   private:
    SimulationConfig fConfig;

   public:
    inline Application() = default;
    explicit Application(const SimulationConfig&);
    Application(int argc, char** argv);

    void ShowUsage() const;

    inline void PrintConfig() const { fConfig.Print(); }

    void LoadConfigFromFile(const SimulationConfig&);
    void InitializeFromCommandLine(int argc, char** argv);

    void UserInitialization();
};

#endif  // RADIATION_TRANSPORT_APPLICATION_H
