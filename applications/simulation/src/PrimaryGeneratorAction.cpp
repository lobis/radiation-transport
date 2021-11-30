
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
#include <G4UnitsTable.hh>
#include <G4VisExtent.hh>
#include <globals.hh>

#include "GlobalManager.h"
#include "OutputManager.h"

using namespace std;

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction(),
      fOutput(OutputManager::Instance()),
      fSourceConfig(GlobalManager::Instance()->GetSimulationConfig().fSourceConfig) {
    spdlog::info("PrimaryGeneratorAction::PrimaryGeneratorAction");
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() = default;

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
    spdlog::debug("PrimaryGeneratorAction::GeneratePrimaries");

    if (!fInitialized) {
        Initialize();
    }

    auto particleName = fSourceConfig.fParticleName;

    fGun.SetParticleDefinition(GetParticle());

    auto particleEnergy = GetEnergy();
    if (particleEnergy < 0) {
        spdlog::error("PrimaryGeneratorAction::GeneratePrimaries - Particle energy cannot be negative");
        exit(1);
    }

    fGun.SetParticleEnergy(particleEnergy * fEnergyScaleFactor);

    auto particlePosition = GetPosition();
    fGun.SetParticlePosition(particlePosition);

    auto particleDirection = GetDirection();
    fGun.SetParticleMomentumDirection(particleDirection);

    if (fSourceConfig.fGeneratorType == "point" || fSourceConfig.fGeneratorType == "plane" || fSourceConfig.fGeneratorType == "disk") {
        /*fGun.SetParticlePosition({fSourceConfig.fGeneratorPosition.x(), fSourceConfig.fGeneratorPosition.y(),
         * fSourceConfig.fGeneratorPosition.z()});*/
    }

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

G4ParticleDefinition* PrimaryGeneratorAction::GetParticle() const {
    const auto& particleName = fSourceConfig.fParticleName;
    const auto& particleExcitedLevel = fSourceConfig.fParticleExcitedLevel;

    if (particleExcitedLevel < 0) {
        spdlog::error("PrimaryGeneratorAction::GetParticle - Particle excitation energy cannot be negative");
        exit(1);
    }

    auto particle = G4ParticleTable::GetParticleTable()->FindParticle(particleName);
    if (particle && particleExcitedLevel > 0) {
        spdlog::error("Particle '{}' with non zero excited level not supported");
        exit(1);
    }
    if (!particle) {
        // there is probably a better way to find the ion
        for (int Z = 1; Z <= 110; Z++) {
            if (particle) {
                break;
            }
            for (int A = 2 * Z - 1; A <= 3 * Z; A++) {
                if (particleName == G4IonTable::GetIonTable()->GetIonName(Z, A)) {
                    particle = G4IonTable::GetIonTable()->GetIon(Z, A, particleExcitedLevel / keV);
                    spdlog::info("PrimaryGeneratorAction::GetParticle - Found ion '{}' with Z={} and A={}", particleName, Z, A);
                    break;
                }
            }
        }
    }

    if (!particle) {
        spdlog::error("PrimaryGeneratorAction::GeneratePrimaries - Could not find particle '{}'", particleName);
        exit(1);
    }

    return particle;
}

double PrimaryGeneratorAction::GetEnergy() const {
    if (fEnergyDistribution) {
        spdlog::debug("PrimaryGeneratorAction::GetEnergy - Generating from Geant4");
        return fEnergyDistribution->GenerateOne(fParticle);
    }

    double energy;
    if (fSourceConfig.fEnergyDistributionType == "mono") {
        energy = fSourceConfig.fEnergyDistributionMonoValue;
    } else {
        spdlog::error("Energy distribution type '{}' sampling not implemented yet", fSourceConfig.fEnergyDistributionType);
        exit(1);
    }

    return energy;
}

G4ThreeVector PrimaryGeneratorAction::GetDirection() const {
    spdlog::debug("PrimaryGeneratorAction::GetDirection - {}", fAngularDistribution ? fAngularDistribution->GetDistType() : "Not defined");
    if (fAngularDistribution) {
        spdlog::debug("PrimaryGeneratorAction::GetDirection - Generating from Geant4");
        auto momentum = fAngularDistribution->GenerateOne();
        return momentum;
    }

    G4ThreeVector direction;
    if (fSourceConfig.fAngularDistributionType == "flux") {
        direction = {fSourceConfig.fAngularDistributionDirection.x(), fSourceConfig.fAngularDistributionDirection.y(),
                     fSourceConfig.fAngularDistributionDirection.z()};
    } else if (fSourceConfig.fAngularDistributionType == "isotropic") {
        //
        spdlog::error("isotropic not implemented");
        exit(1);
    } else {
        spdlog::error("Angular distribution type '{}' sampling not implemented yet", fSourceConfig.fAngularDistributionType);
        exit(1);
    }

    return direction;
}

G4ThreeVector PrimaryGeneratorAction::GetPosition() const {
    if (fPositionDistribution) {
        spdlog::debug("PrimaryGeneratorAction::GetPosition - Generating from Geant4");
        return fPositionDistribution->GenerateOne();
    }

    G4ThreeVector position;
    if (fSourceConfig.fGeneratorType == "point") {
        position = {fSourceConfig.fGeneratorPosition.x(), fSourceConfig.fGeneratorPosition.y(), fSourceConfig.fGeneratorPosition.z()};
    } else {
        spdlog::error("Position distribution type '{}' sampling not implemented yet", fSourceConfig.fGeneratorType);
        exit(1);
    }

    return position;
}

void PrimaryGeneratorAction::Initialize() {
    spdlog::debug("PrimaryGeneratorAction::Initialize");

    fParticle = GetParticle();

    if (!G4UnitDefinition::IsUnitDefined(fSourceConfig.fEnergyDistributionUnit) ||
        G4UnitDefinition::GetCategory(fSourceConfig.fEnergyDistributionUnit) != "Energy") {
        spdlog::error("energy unit '{}' not defined", fSourceConfig.fEnergyDistributionUnit);
        exit(1);
    }
    fEnergyScaleFactor = G4UnitDefinition::GetValueOf(fSourceConfig.fEnergyDistributionUnit);

    if (fSourceConfig.fEnergyDistributionType == "mono") {
        fEnergyDistribution = make_unique<G4SPSEneDistribution>();
        fEnergyDistribution->SetEnergyDisType("Mono");
    } else {
        spdlog::error("Energy distribution type '{}' sampling not implemented yet", fSourceConfig.fEnergyDistributionType);
        exit(1);
    }
    if (fEnergyDistribution) {
        spdlog::info("Geant4 energy distribution: {}", fEnergyDistribution->GetEnergyDisType());
    }

    fPositionDistribution = make_unique<G4SPSPosDistribution>();
    if (fSourceConfig.fGeneratorType == "point") {
        fPositionDistribution->SetPosDisType("Point");
    } else if (fSourceConfig.fGeneratorType == "plane" || fSourceConfig.fGeneratorType == "disk") {
        fPositionDistribution->SetPosDisType("Plane");
    } else {
        spdlog::error("Angular distribution type '{}' sampling not implemented yet", fSourceConfig.fAngularDistributionType);
        exit(1);
    }
    if (fPositionDistribution) {
        spdlog::info("Geant4 position distribution: {}", fPositionDistribution->GetPosDisType());
    }

    if (fSourceConfig.fAngularDistributionType == "flux") {
        //
    } else if (fSourceConfig.fAngularDistributionType == "isotropic") {
        fAngularDistribution = make_unique<G4SPSAngDistribution>();
        fAngularDistribution->SetAngDistType("iso");
    } else {
        spdlog::error("Angular distribution type '{}' sampling not implemented yet", fSourceConfig.fAngularDistributionType);
        exit(1);
    }
    if (fPositionDistribution) {
        spdlog::info("Geant4 angular distribution: {}", fAngularDistribution->GetDistType());
    }

    fInitialized = true;
}