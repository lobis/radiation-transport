//
// Created by lobis on 25/11/2021.
//

#include "DetectorConfig.h"

#include <filesystem>
#include <iostream>

using namespace std;

namespace fs = std::filesystem;

namespace YAML {
template <>
struct convert<DetectorVolume> {
    static Node encode(const DetectorVolume& rhs) {
        Node node;
        node["name"] = rhs.fName;
        if (rhs.fIsSensitive) {
            node["sensitive"] = rhs.fIsSensitive;
        }
        return node;
    }

    static bool decode(const Node& node, DetectorVolume& rhs) {
        if (!node["name"]) {
            return false;
        }
        rhs.fName = node["name"].as<string>();
        if (node["sensitive"]) {
            rhs.fIsSensitive = node["sensitive"].as<bool>();
        }
        return true;
    }
};
}  // namespace YAML

void DetectorConfig::Deserialize(const YAML::Node& node) {
    if (node["geometry"]) {
        fGeometryFilename = node["geometry"].as<string>();
    }

    if (node["checkOverlaps"]) {
        fCheckOverlaps = node["checkOverlaps"].as<bool>();
    }

    if (node["volumes"]) {
        fVolumes = node["volumes"].as<vector<DetectorVolume>>();
    }
}

YAML::Node DetectorConfig::Serialize() const {
    YAML::Node node;

    node["geometry"] = fGeometryFilename;
    node["checkOverlaps"] = fCheckOverlaps;
    node["volumes"] = fVolumes;

    return node;
}

string DetectorConfig::GetGeometryAbsolutePath() const {
    if (fGeometryFilename.empty()) {
        return "";
    }
    if (fs::path(fGeometryFilename).is_absolute()) {
        return fGeometryFilename;
    }
    return fs::path(fConfigAbsolutePath).parent_path() / fGeometryFilename;
}