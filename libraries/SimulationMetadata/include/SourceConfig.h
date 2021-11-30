//
// Created by lobis on 11/25/2021.
//

#ifndef RADIATION_TRANSPORT_SOURCECONFIG_H
#define RADIATION_TRANSPORT_SOURCECONFIG_H

#define INFINITY 1.0E30
#include <TVector3.h>

#include "SerializableConfig.h"

class SourceConfig : public SerializableConfig {
   public:
    void Deserialize(const YAML::Node&) override;
    YAML::Node Serialize() const override;

   private:
    std::set<std::string> fGeneratorTypesAvailable = {"point", "plane", "disk"};
    std::set<std::string> fEnergyDistributionTypesAvailable = {"mono", "linear", "exponential", "power"};

    void DeserializeGenerator(const YAML::Node&);
    void DeserializeEnergy(const YAML::Node&);
    void DeserializeAngular(const YAML::Node&);

    YAML::Node SerializeGenerator() const;
    YAML::Node SerializeEnergy() const;
    YAML::Node SerializeAngular() const;

   public:  // Attributes must follow schema
    std::string fParticleName = "geantino";
    double fParticleExcitedLevel = 0;

    // Generator
    std::string fGeneratorType = "point";

    TVector3 fGeneratorPosition = {0, 0, 0};
    TVector3 fGeneratorDirection = {0, -1, 0};

    TVector3 fGeneratorSize = {0, 0, 0};

    double fGeneratorDiameter = 0;

    // Energy
    std::string fEnergyDistributionType = "mono";

    std::string fEnergyDistributionUnit = "keV";

    double fEnergyDistributionLimitMin = 0;
    double fEnergyDistributionLimitMax = INFINITY;

    double fEnergyDistributionMonoValue = 100;

    // Angular
    std::set<std::string> fAngularDistributionTypesAvailable = {"isotropic", "focused", "flux"};

    std::string fAngularDistributionType = "isotropic";

    double fAngularDistributionLimitThetaMin = 0;
    double fAngularDistributionLimitThetaMax = TMath::Pi();

    double fAngularDistributionLimitPhiMin = 0;
    double fAngularDistributionLimitPhiMax = 2 * TMath::Pi();

    TVector3 fAngularDistributionDirection = {0, -1, 0};
};
#endif  // RADIATION_TRANSPORT_SOURCECONFIG_H
