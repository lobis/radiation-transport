//
// Created by lobis on 25/11/2021.
//

#ifndef RADIATION_TRANSPORT_DETECTORCONFIG_H
#define RADIATION_TRANSPORT_DETECTORCONFIG_H

#include "SerializableConfig.h"

struct DetectorVolume {
    std::string fName;
    bool fIsSensitive = false;

    inline bool operator==(const DetectorVolume& rhs) const { return (rhs.fName == fName && rhs.fIsSensitive == fIsSensitive); }
};

class DetectorConfig : public SerializableConfig {
   public:
    void Deserialize(const YAML::Node&) override;
    YAML::Node Serialize() const override;

   public:  // Attributes must follow schema
    std::string fGeometryFilename;
    bool fCheckOverlaps = false;
    std::vector<DetectorVolume> fVolumes;
};

#endif  // RADIATION_TRANSPORT_DETECTORCONFIG_H
