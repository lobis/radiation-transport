//
// Created by lobis on 24/11/2021.
//

#include "Application.h"

#include <TSystem.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include <G4RunManagerFactory.hh>
#include <G4UIExecutive.hh>
#include <G4UImanager.hh>
#include <G4VisExecutive.hh>

#include "DetectorConstruction.h"

using namespace std;

void Application::Run() {
    spdlog::info("Application::Run");

    auto runManagerType = G4RunManagerType::Default;
    if (fConfig.fRunManagerType == "serial") {
        runManagerType = G4RunManagerType::SerialOnly;
        spdlog::info("Initializing Serial Run Manager");
    } else if (fConfig.fRunManagerType == "multithreading") {
        runManagerType = G4RunManagerType::MTOnly;
        spdlog::info("Initializing Multithreaded Run Manager");
    }
    auto runManager = G4RunManagerFactory::CreateRunManager(runManagerType);

    runManager->SetNumberOfThreads(fConfig.fThreads);

    runManager->SetUserInitialization(new DetectorConstruction(fConfig.GetGeometryAbsolutePath()));

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
    spdlog::info("Application::ShowUsage:");
}

Application::Application(const SimulationConfig& config) { LoadConfigFromFile(config); }

Application::Application(int argc, char** argv) { InitializeFromCommandLine(argc, argv); }

void Application::LoadConfigFromFile(const SimulationConfig& config) {
    spdlog::debug("Application::LoadConfigFromFile");
    fConfig = config;

    // Set the seed
    if (fConfig.fSeed != 0) {
        CLHEP::HepRandom::setTheSeed(fConfig.fSeed);
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
    } else {
        spdlog::debug("config file '{}' found", configFilename);
        LoadConfigFromFile(configFilename);
    }

    while (true) {
        const int option = getopt(argc - 1, argv, "st:v:");
        if (option == -1) break;
        switch (option) {
            case 's':
                fConfig.fRunManagerType = "serial";
                spdlog::info("Command line option (-s): 'serial' mode");
                break;

            case 't':
                spdlog::info("Command line option (-t): 'threads' with value: {}", optarg);
                spdlog::debug("Command line option 'threads' as int: {}", std::stoi(optarg));
                fConfig.fThreads = std::stoi(optarg);
                break;

            case 'v':
                spdlog::info("Command line option (-v): 'verboseLevel' with value: {}", optarg);
                fConfig.SetVerboseLevel(optarg);
                break;

            default:
                // invalid option
                spdlog::error("Error processing command line arguments");
                exit(1);
        }
    }
}
