//
// Created by lobis on 25/11/2021.
//

#ifndef RADIATION_TRANSPORT_SERIALIZABLECONFIG_H
#define RADIATION_TRANSPORT_SERIALIZABLECONFIG_H

#include <yaml-cpp/yaml.h>

#include <iostream>
#include <string>
#include <vector>

class SerializableConfig {
   public:
    inline SerializableConfig() = default;

    virtual void Deserialize(const YAML::Node&) = 0;
    virtual YAML::Node Serialize() const = 0;

    inline virtual void Print() const { std::cout << Serialize() << std::endl; };
};

#endif  // RADIATION_TRANSPORT_SERIALIZABLECONFIG_H
