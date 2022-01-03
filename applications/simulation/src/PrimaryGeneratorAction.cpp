
#include "PrimaryGeneratorAction.h"

#include <TRotation.h>
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
#include <G4Threading.hh>
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

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete fAngularDistributionCustomFunctionCDF;
    delete fEnergyDistributionCustomFunctionCDF;
};

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

    spdlog::error("Angular distribution type '{}' sampling not implemented yet", fSourceConfig.fAngularDistributionType);
    exit(1);
}

G4ThreeVector PrimaryGeneratorAction::GetPosition() const {
    if (fPositionDistribution) {
        spdlog::debug("PrimaryGeneratorAction::GetPosition - Generating from Geant4 - {}", fPositionDistribution->GetPosDisType());
        return fPositionDistribution->GenerateOne();
    }

    spdlog::debug("PrimaryGeneratorAction::GetPosition - NOT Generating from Geant4 - {}", fSourceConfig.fPositionDistributionType);

    spdlog::error("Position distribution type '{}' sampling not implemented yet", fSourceConfig.fPositionDistributionType);
    exit(1);
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
    } else if (fSourceConfig.fEnergyDistributionType == "cosmicMuonsSeaLevel") {
        if (fParticle->GetParticleName() != "mu-" && fParticle->GetParticleName() != "mu+") {
            spdlog::warn("Using '{}' energy distribution for particle '{}'", fSourceConfig.fEnergyDistributionType, fParticle->GetParticleName());
        }
        fEnergyDistribution->SetEnergyDisType("User");
        fEnergyDistribution->ReSetHist("energy");

        // formula in GeV
        auto momentum = TF1("muonMomentumSeaLevel", "TMath::Power(x, -0.4854 - 0.3406 * TMath::Log(x))", 0.2, 328);
        auto energy = TF1("muonEnergySeaLevel", "TMath::Sqrt(muonMomentumSeaLevel(x)^2 + 0.1057^2) - 0.1057", 0.2, 328);
        // More info on https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/GettingStarted/generalParticleSource.html
        fEnergyDistribution->UserEnergyHisto(
            {fSourceConfig.fEnergyDistributionLimitMin * fEnergyScaleFactor, 0});  // left edge of bin (value not used)
        const int n = 1024 - 1;
        const double step = (fSourceConfig.fEnergyDistributionLimitMax - fSourceConfig.fEnergyDistributionLimitMin) / n * fEnergyScaleFactor;
        for (int i = 0; i < n; i++) {
            auto binCenter = (i + 0.5) * step;
            auto binRightBoundary = (i + 1) * step;
            // we input the upper boundary of the bin!
            // value in Gev!
            fEnergyDistribution->UserEnergyHisto({binRightBoundary, energy.Eval(binCenter * fEnergyScaleFactor / 1E6)});
        }
    } else if (fSourceConfig.fEnergyDistributionType == "cosmicNeutronsSeaLevel") {
        if (fParticle->GetParticleName() != "neutron") {
            spdlog::warn("Using '{}' energy distribution for particle '{}'", fSourceConfig.fEnergyDistributionType, fParticle->GetParticleName());
        }
        fEnergyDistribution->SetEnergyDisType("User");
        fEnergyDistribution->ReSetHist("energy");

        // formula in MeV
        auto energy = TF1("neutronEnergySeaLevel",
                          "1.006E-6 * TMath::Exp(-0.3500 * TMath::Power(TMath::Log(x), 2) + 2.1451 * TMath::Log(x))"
                          "+ 1.011E-3 * TMath::Exp(-0.4106 * TMath::Power(TMath::Log(x), 2) -0.6670 * TMath::Log(x))",
                          0.1E0, 10E3);
        // More info on https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/GettingStarted/generalParticleSource.html
        fEnergyDistribution->UserEnergyHisto(
            {fSourceConfig.fEnergyDistributionLimitMin * fEnergyScaleFactor, 0});  // left edge of bin (value not used)
        const int n = 1024 - 1;
        const double step = (fSourceConfig.fEnergyDistributionLimitMax - fSourceConfig.fEnergyDistributionLimitMin) / n * fEnergyScaleFactor;
        for (int i = 0; i < n; i++) {
            auto binCenter = (i + 0.5) * step;
            auto binRightBoundary = (i + 1) * step;
            // we input the upper boundary of the bin!
            // value in Gev!
            fEnergyDistribution->UserEnergyHisto({binRightBoundary, energy.Eval(binCenter * fEnergyScaleFactor / 1E3)});
        }
    } else if (fSourceConfig.fEnergyDistributionType == "userDefined") {
        const auto& formula = fSourceConfig.fUserDefinedEnergyDistributionFormula;
        spdlog::warn("Using user defined energy distribution for particle '{}' with formula: {}", fSourceConfig.fEnergyDistributionType,
                     fParticle->GetParticleName(), formula);

        fEnergyDistribution->SetEnergyDisType("User");
        fEnergyDistribution->ReSetHist("energy");

        // user defined energy formula in user defined units
        auto energy = TF1("userDefinedEnergy", formula.c_str(), fSourceConfig.fEnergyDistributionLimitMin, fSourceConfig.fEnergyDistributionLimitMax);
        //
        fEnergyDistribution->UserEnergyHisto(
            {fSourceConfig.fEnergyDistributionLimitMin * fEnergyScaleFactor, 0});  // left edge of bin (value not used)
        const int n = 1024 - 1;
        const double step = (fSourceConfig.fEnergyDistributionLimitMax - fSourceConfig.fEnergyDistributionLimitMin) / n * fEnergyScaleFactor;
        for (int i = 0; i < n; i++) {
            auto binCenter = (i + 0.5) * step;
            auto binRightBoundary = (i + 1) * step;
            fEnergyDistribution->UserEnergyHisto({binRightBoundary, energy.Eval(binCenter * fEnergyScaleFactor)});
        }
    } else {
        spdlog::error("Energy distribution type '{}' sampling not implemented yet", fSourceConfig.fEnergyDistributionType);
        exit(1);
    }

    auto energyDistString = fEnergyDistribution->GetEnergyDisType();
    if (fEnergyDistribution->GetEnergyDisType() == "Pow") {
        energyDistString += string(TString::Format(" (E^%0.2f)", fEnergyDistribution->Getalpha()));
    }
    spdlog::info("Geant4 energy distribution: {}", energyDistString);

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
            fPositionDistribution->SetHalfX(fSourceConfig.fPositionDistributionRectangleDimensions.x() / 2.0 / fPositionScaleFactor);
            fPositionDistribution->SetHalfY(fSourceConfig.fPositionDistributionRectangleDimensions.y() / 2.0 / fPositionScaleFactor);
            fPositionDistribution->SetHalfZ(fSourceConfig.fPositionDistributionRectangleDimensions.z() / 2.0 / fPositionScaleFactor);
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
    spdlog::info("Geant4 position distribution: {}", fPositionDistribution->GetPosDisType());

    // Angular
    fAngularDistribution->SetMinTheta(fSourceConfig.fAngularDistributionLimitThetaMin);
    fAngularDistribution->SetMaxTheta(fSourceConfig.fAngularDistributionLimitThetaMax);

    fAngularDistribution->SetMinPhi(fSourceConfig.fAngularDistributionLimitPhiMin);
    fAngularDistribution->SetMaxPhi(fSourceConfig.fAngularDistributionLimitPhiMax);

    if (fSourceConfig.fAngularDistributionType == "flux") {
        fAngularDistribution->SetAngDistType("planar");
        const auto& direction = fSourceConfig.fAngularDistributionDirection;
        fAngularDistribution->SetParticleMomentumDirection({direction.x(), direction.y(), direction.z()});
    } else if (fSourceConfig.fAngularDistributionType == "isotropic") {
        fAngularDistribution->SetAngDistType("iso");
    } else if (fSourceConfig.fAngularDistributionType == "cos2") {
        fAngularDistribution->SetAngDistType("user");
        fAngularDistribution->ReSetHist("theta");

        // More info on https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/GettingStarted/generalParticleSource.html
        fAngularDistribution->UserDefAngTheta({0, 0});  // left edge of bin (value not used)
        const int n = 1024 - 1;
        const double step = (TMath::Pi() / 2) / n;
        for (int i = 0; i < n; i++) {
            auto binCenter = (i + 0.5) * step;
            auto binRightBoundary = (i + 1) * step;
            // we input the upper boundary of the bin!
            fAngularDistribution->UserDefAngTheta({binRightBoundary, TMath::Power(TMath::Cos(binCenter), 2)});
        }

    } else {
        fAngularDistribution = nullptr;
        spdlog::error("Angular distribution type '{}' sampling not implemented yet", fSourceConfig.fAngularDistributionType);
        exit(1);
    }
    spdlog::info("Geant4 angular distribution: {}", fAngularDistribution->GetDistType());

    fInitialized = true;
}