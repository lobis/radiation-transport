
#include "PrimaryGeneratorAction.h"

#include <spdlog/spdlog.h>

#include <G4Event.hh>
#include <G4GeneralParticleSource.hh>
#include <G4IonTable.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4RunManager.hh>
#include <G4SDManager.hh>
#include <G4SPSAngDistribution.hh>
#include <G4SystemOfUnits.hh>
#include <G4VisExtent.hh>
#include <globals.hh>

#include "GlobalManager.h"
#include "OutputManager.h"

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction(),
      fOutput(OutputManager::Instance()),
      fSourceConfig(GlobalManager::Instance()->GetSimulationConfig().fSourceConfig) {
    spdlog::info("PrimaryGeneratorAction::PrimaryGeneratorAction");
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() = default;

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
    spdlog::debug("PrimaryGeneratorAction::GeneratePrimaries");

    auto particleName = fSourceConfig.fParticleName;
    auto particle = G4ParticleTable::GetParticleTable()->FindParticle(particleName);
    if (!particle) {
        spdlog::error("PrimaryGeneratorAction::GeneratePrimaries - Could not find particle ''", particleName);
        exit(1);
    }

    fGun.SetParticleDefinition(particle);

    auto particleEnergy = fSourceConfig.fParticleEnergy;
    if (particleEnergy < 0) {
        spdlog::error("PrimaryGeneratorAction::GeneratePrimaries - Particle energy cannot be negative");
    }

    fGun.SetParticleEnergy(particleEnergy * keV);

    if (fSourceConfig.fGeneratorType == "point" || fSourceConfig.fGeneratorType == "plane") {
        fGun.SetParticlePosition({fSourceConfig.fGeneratorPosition.x(), fSourceConfig.fGeneratorPosition.y(), fSourceConfig.fGeneratorPosition.z()});
    }

    fGun.SetParticleMomentumDirection(
        {fSourceConfig.fGeneratorDirection.x(), fSourceConfig.fGeneratorDirection.y(), fSourceConfig.fGeneratorDirection.z()});

    fGun.GeneratePrimaryVertex(event);

    const auto& primaryPosition = event->GetPrimaryVertex()->GetPosition();

    auto primary = event->GetPrimaryVertex()->GetPrimary();

    const auto primaryEnergy = primary->GetKineticEnergy();
    const auto& primaryDirection = primary->GetMomentumDirection();

    spdlog::debug(
        "PrimaryGeneratorAction::GeneratePrimaries - Particle: {} - Position (mm): [{:03.2f}, {:03.2f}, {:03.2f}] - Energy: {:03.2f} keV"
        " - Direction: [{:03.4f}, {:03.4f}, {:03.4f}] ",                               //
        primary->GetParticleDefinition()->GetParticleName(),                           //
        primaryPosition.x() / mm, primaryPosition.y() / mm, primaryPosition.z() / mm,  //
        primaryEnergy / keV,                                                           //
        primaryDirection.x(), primaryDirection.y(), primaryDirection.z()               //
    );
}
