//
// Created by lobis on 22/11/2021.
//

#include <TSystem.h>
#include <spdlog/spdlog.h>
#include <unistd.h>
#include <yaml-cpp/yaml.h>

#include <G4RunManagerFactory.hh>
#include <G4UIExecutive.hh>
#include <G4UImanager.hh>
#include <G4VisExecutive.hh>

#include "SimulationConfig.h"

using namespace std;

void UpdateConfigFromCommandLine(int argc, char** argv, SimulationConfig* config) {
    if (argc <= 1) {
        spdlog::error("You must specify input config file as command line argument!");
        exit(1);
    }
    string configFilename = argv[argc - 1];
    spdlog::info("Input config file: {}", configFilename);
    if (gSystem->AccessPathName(configFilename.c_str(), kFileExists)) {
        spdlog::error("config file '{}' does not exist. cwd: {}", configFilename, getcwd(nullptr, 256));
        exit(1);
    } else {
        spdlog::debug("config file '{}' found", configFilename);
    }

    while (true) {
        const int option = getopt(argc - 1, argv, "st:v:");
        if (option == -1) break;
        switch (option) {
            case 's':
                config->fRunManagerType = "serial";
                spdlog::info("Command line option (-s): 'serial' mode");
                break;

            case 't':
                spdlog::info("Command line option (-t): 'threads' with value: {}", optarg);
                spdlog::debug("Command line option 'threads' as int: {}", std::stoi(optarg));
                config->fThreads = std::stoi(optarg);
                break;

            case 'v':
                spdlog::info("Command line option (-v): 'verboseLevel' with value: {}", optarg);
                config->SetVerboseLevel(optarg);
                break;

            default:
                // invalid option
                spdlog::error("Error processing command line arguments");
                exit(1);
        }
    }
}

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%T][%^%l%$][thread %t]: %v");

    spdlog::info("Starting simulation!");

    auto config = SimulationConfig::LoadFromFile("../analysis/test/files/simulation.yaml");

    UpdateConfigFromCommandLine(argc, argv, &config);

    auto runManagerType = G4RunManagerType::Default;
    if (config.fRunManagerType == "serial") {
        runManagerType = G4RunManagerType::SerialOnly;
        spdlog::info("Initializing Serial Run Manager");
    } else if (config.fRunManagerType == "multithreading") {
        runManagerType = G4RunManagerType::MTOnly;
        spdlog::info("Initializing Multithreaded Run Manager");
    }
    auto runManager = G4RunManagerFactory::CreateRunManager(runManagerType);

    runManager->SetNumberOfThreads(config.fThreads);

    auto UImanager = G4UImanager::GetUIpointer();

    size_t numberOfCommands = config.fCommands.size();
    spdlog::debug("Number of commands defined in config file: {}", numberOfCommands);
    for (size_t i = 0; i < numberOfCommands; i++) {
        string command = config.fCommands[i];
        spdlog::info("--> Command {} of {}: {}", i + 1, numberOfCommands, command);
        UImanager->ApplyCommand(command);
    }

    return 0;
}