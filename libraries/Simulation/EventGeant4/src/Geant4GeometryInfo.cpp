//
// Created by lobis on 03/12/2021.
//

#include "Geant4GeometryInfo.h"

#include <TPRegexp.h>
#include <TXMLEngine.h>
#include <spdlog/spdlog.h>

using namespace std;

namespace myXml {
XMLNodePointer_t FindChildByName(TXMLEngine xml, XMLNodePointer_t node, const TString& name) {
    XMLNodePointer_t child = xml.GetChild(node);
    while (child) {
        TString childName = xml.GetNodeName(child);
        if (childName.EqualTo(name)) {
            return child;
        }
        child = xml.GetNext(child);
    }
    return nullptr;
}
TString GetNodeAttribute(TXMLEngine xml, XMLNodePointer_t node, const TString& attributeName) {
    XMLAttrPointer_t attr = xml.GetFirstAttr(node);
    while (attr) {
        if (TString(xml.GetAttrName(attr)).EqualTo(attributeName)) {
            TString refName = xml.GetAttrValue(attr);
            return refName;
        }
        attr = xml.GetNextAttr(attr);
    }
    return {};
}

void AddVolumesRecursively(vector<TString>* container, const vector<TString>& children, map<TString, TString>& nameTable,
                           map<TString, vector<TString>>& childrenTable, const TString& name = "") {
    // G4cout << "called AddVolumesRecursively with name: " << name << endl;
    for (const auto& child : children) {
        // G4cout << "\t" << child << endl;
    }
    if (children.empty()) {
        container->push_back(name);
        // G4cout << "ADDED: " << name << endl;
        return;
    }
    for (const auto& childName : children) {
        AddVolumesRecursively(container, childrenTable[nameTable[childName]], nameTable, childrenTable, name + "_" + childName);
    }
}
}  // namespace myXml

void Geant4GeometryInfo::PopulateFromGdml(const TString& gdmlFilename) {
    spdlog::info("Geant4GeometryInfo::PopulateFromGdml");
    // Geometry must be in GDML
    TXMLEngine xml;
    XMLDocPointer_t xmldoc = xml.ParseFile(gdmlFilename.Data());
    if (!xmldoc) {
        spdlog::error("Geant4GeometryInfo::PopulateFromGdml - GDML file '{}' not found", gdmlFilename.Data());
        exit(1);
    }
    map<TString, TString> nameTable;
    map<TString, vector<TString>> childrenTable;
    XMLNodePointer_t mainNode = xml.DocGetRootElement(xmldoc);
    XMLNodePointer_t structure = myXml::FindChildByName(xml, mainNode, "structure");
    XMLNodePointer_t child = xml.GetChild(structure);
    while (child) {
        TString name = xml.GetNodeName(child);
        TString volumeName = myXml::GetNodeAttribute(xml, child, "name");
        auto physicalVolumeNode = xml.GetChild(child);
        childrenTable[volumeName] = {};
        while (physicalVolumeNode) {
            auto physicalVolumeName = myXml::GetNodeAttribute(xml, physicalVolumeNode, "name");
            auto volumeRefNode = xml.GetChild(physicalVolumeNode);
            while (volumeRefNode) {
                TString volumeRefNodeName = xml.GetNodeName(volumeRefNode);
                if (volumeRefNodeName.EqualTo("volumeref")) {
                    TString refName = myXml::GetNodeAttribute(xml, volumeRefNode, "ref");
                    nameTable[physicalVolumeName] = refName;
                    childrenTable[volumeName].push_back(physicalVolumeName);
                }
                volumeRefNode = xml.GetNext(volumeRefNode);
            }
            physicalVolumeNode = xml.GetNext(physicalVolumeNode);
        }
        child = xml.GetNext(child);
    }

    fGdmlNewPhysicalNames.clear();
    for (const auto& topName : childrenTable["world"]) {
        auto children = childrenTable[nameTable[topName]];
        myXml::AddVolumesRecursively(&fGdmlNewPhysicalNames, children, nameTable, childrenTable, topName);
    }

    xml.FreeDoc(xmldoc);

    // Find duplicates
    size_t n = fGdmlNewPhysicalNames.size();
    set<TString> s;
    for (const auto& name : fGdmlNewPhysicalNames) {
        s.insert(name);
    }
    if (s.size() != n) {
        spdlog::error("Geant4GeometryInfo::PopulateFromGdml - Generated a duplicate name, please check assembly");
        exit(1);
    }
}

TString Geant4GeometryInfo::GetAlternativeNameFromGeant4PhysicalName(const TString& geant4PhysicalName) const {
    if (fGeant4PhysicalNameToNewPhysicalNameMap.count(geant4PhysicalName) > 0) {
        return fGeant4PhysicalNameToNewPhysicalNameMap.at(geant4PhysicalName);
    }
    return geant4PhysicalName;
}

TString Geant4GeometryInfo::GetGeant4PhysicalNameFromAlternativeName(const TString& name) const {
    for (const auto& [geant4Name, alternativeName] : fGeant4PhysicalNameToNewPhysicalNameMap) {
        if (alternativeName == name) {
            return geant4Name;
        }
    }
    return "";
}

Int_t Geant4GeometryInfo::GetIDFromVolumeName(const TString& volumeName) const {
    for (int i = 0; i < fGdmlNewPhysicalNames.size(); i++) {
        if (volumeName.EqualTo(fGdmlNewPhysicalNames[i])) {
            return i;
        }
    }

    int i = 0;
    for (const auto& physical : GetAllPhysicalVolumes()) {
        if (volumeName.EqualTo(physical)) {
            return i;
        }
        i++;
    }

    spdlog::error("Geant4GeometryInfo::GetIDFromPhysicalName - ID not found for '{}'", volumeName);
    exit(1);
}

void Geant4GeometryInfo::Print() const {
    spdlog::info("Geant4GeometryInfo::Print");

    spdlog::info("Physical volumes:");
    for (const auto& physical : GetAllPhysicalVolumes()) {
        const auto newName = GetAlternativeNameFromGeant4PhysicalName(physical);
        const auto logical = fPhysicalToLogicalVolumeMap.at(physical);
        const auto material = fLogicalToMaterialMap.at(logical);
        const auto position = GetPositionInWorld(physical);

        spdlog::info("\t- {} - Logical: {} - Material: {} - Position: ({:0.1f} {:0.1f} {:0.1f}) mm",
                     (newName == physical ? physical : newName + " (" + physical + ")"), logical, material, position.X(), position.Y(), position.Z());
    }

    spdlog::info("Logical volumes:");
    for (const auto& logical : GetAllLogicalVolumes()) {
        const auto material = fLogicalToMaterialMap.at(logical);
        spdlog::info("\t- {} - Material: {}", logical, material);
    }
}

std::vector<TString> Geant4GeometryInfo::GetAllLogicalVolumes() const {
    auto volumes = std::vector<TString>();

    for (const auto& [logical, _] : fLogicalToPhysicalMap) {
        volumes.emplace_back(logical);
    }

    return volumes;
}

std::vector<TString> Geant4GeometryInfo::GetAllPhysicalVolumes() const {
    auto volumes = std::vector<TString>();

    for (const auto& [physical, _] : fPhysicalToLogicalVolumeMap) {
        volumes.emplace_back(physical);
    }

    return volumes;
}

std::vector<TString> Geant4GeometryInfo::GetAllAlternativePhysicalVolumes() const {
    auto volumes = std::vector<TString>();

    for (const auto& [physical, logical] : fPhysicalToLogicalVolumeMap) {
        volumes.emplace_back(GetAlternativeNameFromGeant4PhysicalName(physical));
    }

    return volumes;
}

std::vector<TString> Geant4GeometryInfo::GetAllPhysicalVolumesMatchingExpression(const TString& regularExpression) const {
    auto volumes = std::vector<TString>();

    TPRegexp regex(regularExpression);

    for (const auto& volume : GetAllPhysicalVolumes()) {
        if (regex.Match(volume)) {
            volumes.emplace_back(volume);
        }
    }

    return volumes;
}

std::vector<TString> Geant4GeometryInfo::GetAllLogicalVolumesMatchingExpression(const TString& regularExpression) const {
    auto volumes = std::vector<TString>();

    TPRegexp regex(regularExpression);

    for (const auto& volume : GetAllLogicalVolumes()) {
        if (regex.Match(volume)) {
            volumes.emplace_back(volume);
        }
    }

    return volumes;
}

TVector3 Geant4GeometryInfo::GetPositionInWorld(const TString& volume) const {
    if (fPhysicalToPositionInWorldMap.count(volume) > 0) {
        return fPhysicalToPositionInWorldMap.at(volume);
    }
    // asked for alternative name
    return fPhysicalToPositionInWorldMap.at(GetGeant4PhysicalNameFromAlternativeName(volume));
    // may throw if asked for invalid name
}
