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

#include "spdlog/spdlog.h"

PhysicsList::PhysicsList(G4int verbosity) : G4VModularPhysicsList() {
    SetVerboseLevel(verbosity);

    SetDefaultCutValue(0.5 / mm);
    spdlog::debug("PhysicsList::PhysicsList");

    // EM physics
    fEmPhysicsList = new G4EmLivermorePhysics(verboseLevel);
    fDecayPhysicsList = new G4DecayPhysics(verboseLevel);
    fRadioactiveDecayPhysicsList = new G4RadioactiveDecayPhysics(verboseLevel);

    fHadronPhys.push_back(new G4HadronElasticPhysicsHP(verboseLevel));
    fHadronPhys.push_back(new G4IonBinaryCascadePhysics(verboseLevel));
    fHadronPhys.push_back(new G4HadronPhysicsQGSP_BIC_HP(verboseLevel));
    // fHadronPhys.push_back(new G4NeutronTrackingCut(verboseLevel));
    fHadronPhys.push_back(new G4EmExtraPhysics(verboseLevel));

    // register
    RegisterPhysics(fEmPhysicsList);
    RegisterPhysics(fDecayPhysicsList);
    RegisterPhysics(fRadioactiveDecayPhysicsList);

    for (auto& fHadronPhy : fHadronPhys) {
        RegisterPhysics(fHadronPhy);
    }
}

void PhysicsList::ConstructParticle() { G4VModularPhysicsList::ConstructParticle(); }

void PhysicsList::ConstructProcess() { G4VModularPhysicsList::ConstructProcess(); }

void PhysicsList::SetCuts() {
    return;
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(100. * eV, 100. * TeV);

    SetCutsWithDefault();

    G4Region* region = G4RegionStore::GetInstance()->GetRegion("Garfield");
    if (region) {
        auto cuts = new G4ProductionCuts();
        cuts->SetProductionCut(1 * um, G4ProductionCuts::GetIndex("gamma"));
        cuts->SetProductionCut(1 * um, G4ProductionCuts::GetIndex("e-"));
        cuts->SetProductionCut(1 * um, G4ProductionCuts::GetIndex("e+"));
        region->SetProductionCuts(cuts);
    }
    DumpCutValuesTable();
}
