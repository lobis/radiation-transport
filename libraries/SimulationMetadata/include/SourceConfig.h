//
// Created by lobis on 11/25/2021.
//

#ifndef RADIATION_TRANSPORT_SOURCECONFIG_H
#define RADIATION_TRANSPORT_SOURCECONFIG_H

#include "SerializableConfig.h"

class SourceConfig : public SerializableConfig {
   public:
    void Deserialize(const YAML::Node&) override;
    YAML::Node Serialize() const override;

   public:  // Attributes must follow schema
};
#endif  // RADIATION_TRANSPORT_SOURCECONFIG_H
