//
// Created by lobis on 25/11/2021.
//

#include "PhysicsList.h"

#include <G4DecayPhysics.hh>
#include <G4EmConfigurator.hh>
#include <G4EmExtraPhysics.hh>
#include <G4EmLivermorePhysics.hh>
#include <G4EmLowEPPhysics.hh>
#include <G4EmPenelopePhysics.hh>
#include <G4EmStandardPhysics.hh>
#include <G4EmStandardPhysics_option4.hh>
#include <G4FastSimulationManagerProcess.hh>
#include <G4HadronElasticPhysics.hh>
#include <G4HadronElasticPhysicsHP.hh>
#include <G4HadronInelasticQBBC.hh>
#include <G4HadronPhysicsQGSP_BERT_HP.hh>
#include <G4HadronPhysicsQGSP_BIC_HP.hh>
#include <G4IonBinaryCascadePhysics.hh>
#include <G4IonPhysics.hh>
#include <G4LossTableManager.hh>
#include <G4Material.hh>
#include <G4MaterialTable.hh>
#include <G4NeutronTrackingCut.hh>
#include <G4OpticalPhysics.hh>
#include <G4PAIModel.hh>
#include <G4PAIPhotModel.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleTypes.hh>
#include <G4ParticleWithCuts.hh>
#include <G4ProcessManager.hh>
#include <G4ProcessVector.hh>
#include <G4RadioactiveDecayPhysics.hh>
#include <G4RegionStore.hh>
#include <G4Scintillation.hh>
#include <G4StoppingPhysics.hh>
#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4ios.hh>
#include <globals.hh>

#include "Exceptions.h"
#include "spdlog/spdlog.h"

PhysicsList::PhysicsList(const PhysicsListConfig& config) : G4VModularPhysicsList(), fConfig(config) {
    spdlog::debug("PhysicsList::PhysicsList");

    SetVerboseLevel(fConfig.fVerbosity);

    SetDefaultCutValue(config.fLengthProductionCutsGlobal / mm);

    for (const auto& physicsListName : fConfig.fPhysicsLists) {
        if (physicsListName == "G4EmLivermorePhysics") {
            fEmPhysicsList = new G4EmLivermorePhysics(fConfig.fVerbosity);
        } else if (physicsListName == "G4DecayPhysics") {
            fDecayPhysicsList = new G4DecayPhysics(fConfig.fVerbosity);
        } else if (physicsListName == "G4RadioactiveDecayPhysics") {
            fRadioactiveDecayPhysicsList = new G4RadioactiveDecayPhysics(fConfig.fVerbosity);
        } else if (physicsListName == "G4HadronElasticPhysicsHP") {
            fHadronPhys.push_back(new G4HadronElasticPhysicsHP(fConfig.fVerbosity));
        } else if (physicsListName == "G4IonBinaryCascadePhysics") {
            fHadronPhys.push_back(new G4IonBinaryCascadePhysics(fConfig.fVerbosity));
        } else if (physicsListName == "G4HadronPhysicsQGSP_BIC_HP") {
            fHadronPhys.push_back(new G4HadronPhysicsQGSP_BIC_HP(fConfig.fVerbosity));
        } else if (physicsListName == "G4NeutronTrackingCut") {
            fHadronPhys.push_back(new G4NeutronTrackingCut(fConfig.fVerbosity));
        } else if (physicsListName == "G4EmExtraPhysics") {
            fHadronPhys.push_back(new G4EmExtraPhysics(fConfig.fVerbosity));
        } else {
            spdlog::error("PhysicsList::PhysicsList: Bad physics list: '{}'", physicsListName);
            throw exceptions::BadPhysicsList;
        }
    }

    // register
    if (fEmPhysicsList) {
        RegisterPhysics(fEmPhysicsList);
    }
    if (fDecayPhysicsList) {
        RegisterPhysics(fDecayPhysicsList);
    }
    if (fRadioactiveDecayPhysicsList) {
        RegisterPhysics(fRadioactiveDecayPhysicsList);
    }
    for (auto& fHadronPhy : fHadronPhys) {
        RegisterPhysics(fHadronPhy);
    }
}

void PhysicsList::ConstructParticle() { G4VModularPhysicsList::ConstructParticle(); }

void PhysicsList::ConstructProcess() { G4VModularPhysicsList::ConstructProcess(); }

void PhysicsList::SetCuts() {
    SetCutsWithDefault();

    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(fConfig.fEnergyProductionCutsMin * eV, fConfig.fEnergyProductionCutsMax * TeV);

    /*
    G4Region* region = G4RegionStore::GetInstance()->GetRegion("Garfield");
    if (region) {
        auto cuts = new G4ProductionCuts();
        cuts->SetProductionCut(1 * um, G4ProductionCuts::GetIndex("gamma"));
        cuts->SetProductionCut(1 * um, G4ProductionCuts::GetIndex("e-"));
        cuts->SetProductionCut(1 * um, G4ProductionCuts::GetIndex("e+"));
        region->SetProductionCuts(cuts);
    }
    DumpCutValuesTable();
    */
}
