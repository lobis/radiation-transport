//
// Created by lobis on 24/11/2021.
//

#ifndef RADIATION_TRANSPORT_APPLICATION_H
#define RADIATION_TRANSPORT_APPLICATION_H

#include "GlobalManager.h"
#include "SimulationConfig.h"

class G4VisManager;
class G4UIExecutive;
class G4RunManager;

class Application {
   private:
    SimulationConfig fConfig;
    G4RunManager* fRunManager;
    G4VisManager* fG4VisManager = nullptr;
    G4UIExecutive* fG4UIExecutive = nullptr;

    void SetLoggingLevelFromConfig();

   public:
    inline Application() : fGlobalManager(GlobalManager::Instance()) { spdlog::set_pattern("[%T][%^%l%$]: %v"); };
    explicit Application(const SimulationConfig&);
    Application(int argc, char** argv);

    void ShowUsage() const;

    inline void PrintConfig() const { fConfig.Print(); }

    void LoadConfigFromFile(const SimulationConfig&);
    void InitializeFromCommandLine(int argc, char** argv);

    void UserInitialization();
    void Initialize();

    GlobalManager* fGlobalManager;
};

#endif  // RADIATION_TRANSPORT_APPLICATION_H
