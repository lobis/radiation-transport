//
// Created by lobis on 25/11/2021.
//

#ifndef RADIATION_TRANSPORT_SERIALIZABLECONFIG_H
#define RADIATION_TRANSPORT_SERIALIZABLECONFIG_H

#include <TROOT.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <string>
#include <vector>

class SerializableConfig {
   public:
    inline SerializableConfig() = default;
    inline ~SerializableConfig() = default;

    virtual void Deserialize(const YAML::Node&) = 0;
    virtual YAML::Node Serialize() const = 0;

    virtual void Print() const;

    static std::string GetAbsolutePath(const std::string&);
    static std::string GetAbsolutePathFromConfig(const std::string& input, const std::string& configFilepath);
};

#endif  // RADIATION_TRANSPORT_SERIALIZABLECONFIG_H
