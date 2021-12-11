//
// Created by lobis on 25/11/2021.
//

#ifndef RADIATION_TRANSPORT_DETECTORCONSTRUCTIONCONFIG_H
#define RADIATION_TRANSPORT_DETECTORCONSTRUCTIONCONFIG_H

#include "SerializableConfig.h"

class DetectorConstructionConfig : public SerializableConfig {
    ClassDef(DetectorConstructionConfig, 1);

   public:
    inline DetectorConstructionConfig() = default;
    inline ~DetectorConstructionConfig() = default;

    void Deserialize(const YAML::Node&) override;
    YAML::Node Serialize() const override;

   public:
    struct DetectorVolume {
        std::string fName;
        bool fIsSensitive = false;

        inline bool operator==(const DetectorVolume& rhs) const { return (rhs.fName == fName); }
    };

   public:  // Attributes must follow schema
    std::string fGeometryFilename;

    bool fCheckOverlaps = false;
    std::vector<DetectorVolume> fVolumes;  // unique values by name

   public:
    std::string fConfigAbsolutePath;
    std::string GetGeometryAbsolutePath() const;

    bool fKeepOnlyTracksInTheseVolumes = false;
    std::vector<std::string> fKeepOnlyTracksInTheseVolumesList = {};
};

#endif  // RADIATION_TRANSPORT_DETECTORCONSTRUCTIONCONFIG_H
