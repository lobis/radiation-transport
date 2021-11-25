//
// Created by lobis on 11/25/2021.
//

#ifndef RADIATION_TRANSPORT_SOURCECONFIG_H
#define RADIATION_TRANSPORT_SOURCECONFIG_H

#include <TVector3.h>

#include "SerializableConfig.h"

class SourceConfig : public SerializableConfig {
   public:
    void Deserialize(const YAML::Node&) override;
    YAML::Node Serialize() const override;

   public:  // Attributes must follow schema
    std::string fParticleName = "geantino";
    double fParticleEnergy = 100 /* keV */;
    double fParticleExcitedLevel = 0;

    std::string fGeneratorType = "point";
    TVector3 fGeneratorPosition;
    TVector3 fGeneratorDirection;
};
#endif  // RADIATION_TRANSPORT_SOURCECONFIG_H
