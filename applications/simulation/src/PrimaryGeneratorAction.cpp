
#include "PrimaryGeneratorAction.h"

#include <TF1.h>
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

    fEnergyDistribution = fSPS.GetEneDist();
    fAngularDistribution = fSPS.GetAngDist();
    fPositionDistribution = fSPS.GetPosDist();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() = default;

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
    spdlog::debug("PrimaryGeneratorAction::GeneratePrimaries");

    if (!fInitialized) {
        Initialize();
    }

    auto particleName = fSourceConfig.fParticleName;

    auto particleEnergy = GetEnergy();
    if (particleEnergy < 0) {
        spdlog::error("PrimaryGeneratorAction::GeneratePrimaries - Particle energy cannot be negative");
        exit(1);
    }

    fGun.SetParticleEnergy(particleEnergy);

    auto particlePosition = GetPosition();
    fGun.SetParticlePosition(particlePosition);

    auto particleDirection = GetDirection();
    fGun.SetParticleMomentumDirection(particleDirection);

    if (fSourceConfig.fPositionDistributionType == "point" || fSourceConfig.fPositionDistributionType == "plane" ||
        fSourceConfig.fPositionDistributionType == "disk") {
        /*fSPS.SetParticlePosition({fSourceConfig.fGeneratorPosition.x(), fSourceConfig.fGeneratorPosition.y(),
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
        spdlog::debug("PrimaryGeneratorAction::GetEnergy - Generating from Geant4 - {}", fEnergyDistribution->GetEnergyDisType());
        return fEnergyDistribution->GenerateOne(fParticle);
    }

    spdlog::debug("PrimaryGeneratorAction::GetEnergy - NOT Generating from Geant4 - {}", fSourceConfig.fEnergyDistributionType);

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
    if (fAngularDistribution) {
        spdlog::debug("PrimaryGeneratorAction::GetDirection - Generating from Geant4 - {}", fAngularDistribution->GetDistType());
        return fAngularDistribution->GenerateOne();
    }

    spdlog::debug("PrimaryGeneratorAction::GetDirection - NOT Generating from Geant4 - {}", fSourceConfig.fAngularDistributionType);

    G4ThreeVector direction;
    if (fSourceConfig.fAngularDistributionType == "cos2") {
        // TODO: NOT WORKING YET
        auto random = G4UniformRand();  // ~U(0,1)
        auto phi = random * 2 * TMath::Pi();

        TF1 cos2("cos2", "cos(x^2)", 0., TMath::Pi() / 2);
        auto theta = cos2.GetRandom();

        direction = G4ThreeVector({TMath::Cos(phi) * TMath::Sin(theta),  //
                                   TMath::Sin(phi) * TMath::Sin(theta),  //
                                   TMath::Cos(theta)})
                        .unit();
    } else {
        spdlog::error("Angular distribution type '{}' sampling not implemented yet", fSourceConfig.fAngularDistributionType);
        exit(1);
    }

    return direction;
}

G4ThreeVector PrimaryGeneratorAction::GetPosition() const {
    if (fPositionDistribution) {
        spdlog::debug("PrimaryGeneratorAction::GetPosition - Generating from Geant4 - {}", fPositionDistribution->GetPosDisType());
        return fPositionDistribution->GenerateOne();
    }

    spdlog::debug("PrimaryGeneratorAction::GetPosition - NOT Generating from Geant4 - {}", fSourceConfig.fPositionDistributionType);

    G4ThreeVector position;

    spdlog::error("Position distribution type '{}' sampling not implemented yet", fSourceConfig.fPositionDistributionType);
    exit(1);

    return position;
}

void PrimaryGeneratorAction::Initialize() {
    spdlog::debug("PrimaryGeneratorAction::Initialize");

    fParticle = GetParticle();

    fSPS.SetParticleDefinition(fParticle);
    fGun.SetParticleDefinition(fSPS.GetParticleDefinition());

    // Energy
    if (!G4UnitDefinition::IsUnitDefined(fSourceConfig.fEnergyDistributionUnit) ||
        G4UnitDefinition::GetCategory(fSourceConfig.fEnergyDistributionUnit) != "Energy") {
        spdlog::error("energy unit '{}' not defined", fSourceConfig.fEnergyDistributionUnit);
        exit(1);
    }
    fEnergyScaleFactor = G4UnitDefinition::GetValueOf(fSourceConfig.fEnergyDistributionUnit);

    fEnergyDistribution->SetEmin(fSourceConfig.fEnergyDistributionLimitMin * fEnergyScaleFactor);
    fEnergyDistribution->SetEmax(fSourceConfig.fEnergyDistributionLimitMax * fEnergyScaleFactor);

    if (fSourceConfig.fEnergyDistributionType == "mono") {
        fEnergyDistribution->SetEnergyDisType("Mono");
        fEnergyDistribution->SetMonoEnergy(fSourceConfig.fEnergyDistributionMonoValue * fEnergyScaleFactor);
    } else {
        fEnergyDistribution = nullptr;
        spdlog::error("Energy distribution type '{}' sampling not implemented yet", fSourceConfig.fEnergyDistributionType);
        exit(1);
    }
    if (fEnergyDistribution) {
        spdlog::info("Geant4 energy distribution: {}", fEnergyDistribution->GetEnergyDisType());
    }

    // Position
    if (!G4UnitDefinition::IsUnitDefined(fSourceConfig.fPositionDistributionUnit) ||
        G4UnitDefinition::GetCategory(fSourceConfig.fPositionDistributionUnit) != "Length") {
        spdlog::error("length unit '{}' not defined", fSourceConfig.fPositionDistributionUnit);
        exit(1);
    }
    fPositionScaleFactor = G4UnitDefinition::GetValueOf(fSourceConfig.fPositionDistributionUnit);

    auto position = fSourceConfig.fPositionDistributionCenter;
    fPositionDistribution->SetCentreCoords(
        {position.x() / fPositionScaleFactor, position.y() / fPositionScaleFactor, position.z() / fPositionScaleFactor});
    if (fSourceConfig.fPositionDistributionType == "point") {
        fPositionDistribution->SetPosDisType("Point");
    } else if (fSourceConfig.fPositionDistributionType == "rectangle" || fSourceConfig.fPositionDistributionType == "square" ||
               fSourceConfig.fPositionDistributionType == "disk") {
        // TODO: Not fully working yet
        fPositionDistribution->SetPosDisType("Plane");
        fPositionDistribution->SetPosRot1({1, 0, 0});
        fPositionDistribution->SetPosRot2({1, 0, 1});
        if (fSourceConfig.fPositionDistributionType == "rectangle") {
            fPositionDistribution->SetPosDisShape("Rectangle");
            fPositionDistribution->SetHalfX(fSourceConfig.fPositionDistributionRectangleSideLong / 2.0 / fPositionScaleFactor);
            fPositionDistribution->SetHalfY(fSourceConfig.fPositionDistributionRectangleSideShort / 2.0 / fPositionScaleFactor);
            fPositionDistribution->SetHalfZ(fSourceConfig.fPositionDistributionRectangleSideShort / 2.0 / fPositionScaleFactor);
        } else if (fSourceConfig.fPositionDistributionType == "square") {
            fPositionDistribution->SetPosDisShape("Square");
            fPositionDistribution->SetHalfX(fSourceConfig.fPositionDistributionSquareSide / 2.0 / fPositionScaleFactor);
            fPositionDistribution->SetHalfY(fSourceConfig.fPositionDistributionSquareSide / 2.0 / fPositionScaleFactor);
            fPositionDistribution->SetHalfZ(fSourceConfig.fPositionDistributionSquareSide / 2.0 / fPositionScaleFactor);
        } else if (fSourceConfig.fPositionDistributionType == "disk") {
            fPositionDistribution->SetPosDisShape("Circle");
            fPositionDistribution->SetRadius(fSourceConfig.fPositionDistributionDiameter / 2.0 / fPositionScaleFactor);
        }
    } else {
        fPositionDistribution = nullptr;
        spdlog::error("cannot process '{}' position dist yet", fSourceConfig.fPositionDistributionType);
        exit(1);
    }
    if (fPositionDistribution) {
        spdlog::info("Geant4 position distribution: {}", fPositionDistribution->GetPosDisType());
    }

    // Angular
    fAngularDistribution->SetMinTheta(fSourceConfig.fAngularDistributionLimitThetaMin);
    fAngularDistribution->SetMaxTheta(fSourceConfig.fAngularDistributionLimitThetaMax);

    fAngularDistribution->SetMinTheta(fSourceConfig.fAngularDistributionLimitPhiMin);
    fAngularDistribution->SetMaxTheta(fSourceConfig.fAngularDistributionLimitPhiMax);

    if (fSourceConfig.fAngularDistributionType == "flux") {
        fAngularDistribution->SetAngDistType("planar");
        auto direction = fSourceConfig.fAngularDistributionDirection;
        fAngularDistribution->SetParticleMomentumDirection({direction.x(), direction.y(), direction.z()});
    } else if (fSourceConfig.fAngularDistributionType == "isotropic") {
        fAngularDistribution->SetAngDistType("iso");
    } else if (fSourceConfig.fAngularDistributionType == "cos2") {
        fAngularDistribution = nullptr;
        spdlog::info("NON Geant4 angular distribution: {}", fSourceConfig.fAngularDistributionType);
    } else {
        fAngularDistribution = nullptr;
        spdlog::error("Angular distribution type '{}' sampling not implemented yet", fSourceConfig.fAngularDistributionType);
        exit(1);
    }
    if (fAngularDistribution) {
        spdlog::info("Geant4 angular distribution: {}", fAngularDistribution->GetDistType());
    }

    fInitialized = true;
}