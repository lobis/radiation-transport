//
// Created by lobis on 24/11/2021.
//

#include "Application.h"

#include <SteppingVerbose.h>
#include <TSystem.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include <G4RunManager.hh>
#include <G4RunManagerFactory.hh>
#include <G4Threading.hh>
#include <G4UIExecutive.hh>
#include <G4UImanager.hh>
#include <G4VSteppingVerbose.hh>
#include <G4VisExecutive.hh>

#include "ActionInitialization.h"
#include "DetectorConstruction.h"
#include "PhysicsList.h"

using namespace std;

void Application::UserInitialization() {
    spdlog::info("Application::UserInitialization");

    SetLoggingLevelFromConfig();

    G4VSteppingVerbose::SetInstance(new SteppingVerbose);

    auto runManagerType = G4RunManagerType::Default;
    if (fConfig.fRunManagerType == "serial") {
        runManagerType = G4RunManagerType::SerialOnly;
        spdlog::info("Initializing Serial Run Manager");
    } else if (fConfig.fRunManagerType == "multithreading") {
        runManagerType = G4RunManagerType::MTOnly;
        spdlog::info("Initializing Multithreaded Run Manager");
    }

    fRunManager = G4RunManagerFactory::CreateRunManager(runManagerType);

    if (G4Threading::IsMultithreadedApplication()) {
        spdlog::debug("Multithreaded Application");
        spdlog::set_pattern("[%T][%^%l%$][thread %t]: %v");
    } else {
        spdlog::debug("Serial Application");
        spdlog::set_pattern("[%T][%^%l%$]: %v");
    }

    fRunManager->SetNumberOfThreads(fConfig.fThreads);

    fRunManager->SetUserInitialization(new DetectorConstruction(fConfig.fDetectorConfig));
    fRunManager->SetUserInitialization(new PhysicsList(fConfig.fPhysicsListConfig));
    fRunManager->SetUserInitialization(new ActionInitialization);

    if (fG4VisManager && fG4UIExecutive) {
        spdlog::info("Running visualization macros");
        Application::RunMacro(Application::fMacroVis);
        if (fG4UIExecutive->IsGUI()) {
            Application::RunMacro(Application::fMacroGUI);
        };
        fG4UIExecutive->SessionStart();
        delete fG4UIExecutive;
    }
}

void Application::Initialize() {
    fRunManager->Initialize();

    auto UImanager = G4UImanager::GetUIpointer();

    size_t numberOfCommands = fConfig.fCommands.size();
    spdlog::debug("Number of commands defined in config file: {}", numberOfCommands);
    for (size_t i = 0; i < numberOfCommands; i++) {
        string command = fConfig.fCommands[i];
        spdlog::info("--> Command {} of {}: {}", i + 1, numberOfCommands, command);
        UImanager->ApplyCommand(command);
    }
}

void Application::ShowUsage() const {
    // TODO
    spdlog::info("Application::ShowUsage");
}

Application::Application(const SimulationConfig& config) : Application() { LoadConfigFromFile(config); }

Application::Application(int argc, char** argv) : Application() { InitializeFromCommandLine(argc, argv); }

void Application::LoadConfigFromFile(const SimulationConfig& config) {
    spdlog::debug("Application::LoadConfigFromFile");
    fConfig = config;
    fGlobalManager->SetSimulationConfig(config);
    //  Set the seed
    if (fConfig.fSeed != 0) {
        CLHEP::HepRandom::setTheSeed(fConfig.fSeed);
        spdlog::info("Setting random seed to: '{}'", fConfig.fSeed);
    }
}

void Application::InitializeFromCommandLine(int argc, char** argv) {
    spdlog::debug("Application::InitializeFromCommandLine");
    if (argc <= 1) {
        spdlog::error("You must specify input config file as command line argument!");
        exit(1);
    }
    string configFilename = argv[argc - 1];
    spdlog::info("Input config file: {}", configFilename);
    if (gSystem->AccessPathName(configFilename.c_str(), kFileExists)) {
        spdlog::error("config file '{}' does not exist. cwd: {}", configFilename, get_current_dir_name());
        exit(1);
    }
    spdlog::debug("config file '{}' found", configFilename);
    LoadConfigFromFile(configFilename);

    while (true) {
        const int option = getopt(argc - 1, argv, "st:v:i");
        if (option == -1) break;
        switch (option) {
            case 's':
                fConfig.fRunManagerType = "serial";
                spdlog::info("Command line option (-s): 'serial' mode");
                break;

            case 't':
                spdlog::info("Command line option (-t): 'threads' with value: {}", optarg);
                spdlog::debug("Command line option 'threads' as int: {}", std::stoi(optarg));
                spdlog::warn("User explicitly set number of threads, setting app run mode as MT");
                fConfig.fRunManagerType = "multithreading";
                fConfig.fThreads = std::stoi(optarg);
                break;

            case 'v':
                spdlog::info("Command line option (-v): 'verboseLevel' with value: {}", optarg);
                fConfig.fVerboseLevel = optarg;
                break;

            case 'i':
                spdlog::info("Command line option (-i): 'interactive'");
                fConfig.fInteractive = true;
                break;

            default:
                // invalid option
                spdlog::error("Error processing command line arguments");
                exit(1);
        }
    }

    if (fConfig.fInteractive) {
        spdlog::info("Initializing G4VisExecutive");
        fG4VisManager = new G4VisExecutive();
        fG4VisManager->Initialize();
        fG4UIExecutive = new G4UIExecutive(argc, argv);
    }
}

void Application::SetLoggingLevelFromConfig() {
    auto verboseLevel = fConfig.fVerboseLevel;
    if (verboseLevel == "debug") {
        spdlog::set_level(spdlog::level::debug);
    } else if (verboseLevel == "info") {
        spdlog::set_level(spdlog::level::info);
    } else if (verboseLevel == "warning") {
        spdlog::set_level(spdlog::level::warn);
    } else if (verboseLevel == "error") {
        spdlog::set_level(spdlog::level::err);
    }
}

void Application::RunMacro(const vector<string>& macroLines) {
    for (const auto& command : macroLines) {
        if (command.empty()) {
            continue;
        }
        spdlog::info("Application::RunMacro - \033[1;42m{}\033[0m", command);
        G4UImanager::GetUIpointer()->ApplyCommand(command);
    }
}

std::vector<std::string> Application::fMacroVis = {
    "/control/verbose 2",
    "/run/verbose 2",
    "/run/initialize",

    "/vis/open OGL 800x600-0+0",

    "/vis/viewer/set/autoRefresh false",
    "/vis/verbose errors",
    "/vis/drawVolume",
    "/vis/viewer/set/viewpointThetaPhi 90. 0.",
    "/vis/viewer/zoom 1.5",
    //"/vis/viewer/set/style wireframe" # surface
    "/vis/scene/endOfEventAction accumulate",
    "/vis/scene/add/trajectories smooth",
    "/vis/modeling/trajectories/create/drawByCharge",
    "/vis/modeling/trajectories/drawByCharge-0/default/setDrawStepPts true",
    "/vis/modeling/trajectories/drawByCharge-0/default/setStepPtsSize 2",

    "/vis/viewer/set/autoRefresh true",
    "/vis/verbose warnings",
};

std::vector<std::string> Application::fMacroGUI = {
    "/gui/addMenu file File",
    "/gui/addButton file Quit exit",
    "/gui/addMenu run Run",
    R"(/gui/addButton run "beamOn 1" "/run/beamOn 1")",
    "/gui/addMenu gun Gun",
    R"(/gui/addButton gun "5 keV"   "/gun/energy 5 keV")",
    R"(/gui/addButton gun "50 keV"   "/gun/energy 50 keV")",
    R"(/gui/addButton gun "250 keV"   "/gun/energy 250 keV")",
    R"(/gui/addButton gun "1 MeV"   "/gun/energy 1 MeV")",
    R"(/gui/addButton gun "opticalphoton"      "/gun/particle opticalphoton")",
    R"(/gui/addButton gun "e-"      "/gun/particle e-")",
    R"(/gui/addButton gun "gamma"      "/gun/particle gamma")",
    R"(/gui/addButton gun "neutron" "/gun/particle neutron")",
    R"(/gui/addButton gun "proton"  "/gun/particle proton")",
    "/gui/addMenu viewer Viewer",
    R"(/gui/addButton viewer "Set style surface" "/vis/viewer/set/style surface")",
    R"(/gui/addButton viewer "Set style wireframe" "/vis/viewer/set/style wireframe")",
    R"(/gui/addButton viewer "Refresh viewer" "/vis/viewer/refresh")",
    "/gui/addButton viewer \"Update viewer (interaction or end-of-file)\" \"/vis/viewer/update\"",
    "/gui/addButton viewer \"Flush viewer (= refresh + update)\" \"/vis/viewer/flush\"",
    R"(/gui/addButton viewer "Update scene" "/vis/scene/notifyHandlers")",
};