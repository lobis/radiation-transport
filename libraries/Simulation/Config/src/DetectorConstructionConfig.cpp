//
// Created by lobis on 25/11/2021.
//

#include "DetectorConstructionConfig.h"

#include <iostream>

ClassImp(DetectorConstructionConfig);

using namespace std;

namespace YAML {
template <>
struct convert<DetectorConstructionConfig::DetectorVolume> {
    static Node encode(const DetectorConstructionConfig::DetectorVolume& rhs) {
        Node node;
        node["name"] = rhs.fName;
        if (rhs.fIsSensitive) {
            node["sensitive"] = rhs.fIsSensitive;
        }
        if (rhs.fKill) {
            node["kill"] = rhs.fKill;
        }
        return node;
    }

    static bool decode(const Node& node, DetectorConstructionConfig::DetectorVolume& rhs) {
        if (!node["name"]) {
            return false;
        }
        rhs.fName = node["name"].as<string>();
        if (node["sensitive"]) {
            rhs.fIsSensitive = node["sensitive"].as<bool>();
        }
        if (node["kill"]) {
            rhs.fKill = node["kill"].as<bool>();
        }
        return true;
    }
};
}  // namespace YAML

void DetectorConstructionConfig::Deserialize(const YAML::Node& node) {
    if (node["geometry"]) {
        fGeometryFilename = node["geometry"].as<string>();
    }

    if (node["checkOverlaps"]) {
        fCheckOverlaps = node["checkOverlaps"].as<bool>();
    }

    if (node["volumes"]) {
        auto volumes = node["volumes"].as<vector<DetectorVolume>>();
        for (auto& volume : volumes) {
            for (auto& existingVolume : fVolumes) {
                if (volume == existingVolume) {
                    continue;
                }
            }
            fVolumes.push_back(volume);
        }
    }

    if (node["keepOnlyTracksInTheseVolumes"]) {
        fKeepOnlyTracksInTheseVolumesList = node["keepOnlyTracksInTheseVolumes"].as<vector<string>>();
    }

    if (node["replaceMaterials"]) {
        for (const auto& replacement : node["replaceMaterials"]) {
            auto logicalVolumeName = replacement.first.as<string>();
            auto newMaterial = replacement.second.as<string>();

            fReplaceMaterialsMap[logicalVolumeName] = newMaterial;
        }
    }
}

YAML::Node DetectorConstructionConfig::Serialize() const {
    YAML::Node node;

    node["geometry"] = fGeometryFilename;
    node["checkOverlaps"] = fCheckOverlaps;
    node["volumes"] = fVolumes;

    if (!fKeepOnlyTracksInTheseVolumesList.empty()) {
        node["keepOnlyTracksInTheseVolumes"] = fKeepOnlyTracksInTheseVolumesList;
    }

    if (!fReplaceMaterialsMap.empty()) {
        node["replaceMaterials"] = fReplaceMaterialsMap;
    }

    return node;
}

string DetectorConstructionConfig::GetGeometryAbsolutePath() const { return GetAbsolutePathFromConfig(fGeometryFilename, fConfigAbsolutePath); }
