//
// Created by lobis on 03/12/2021.
//

#include "DataEventHeader.h"

#include <TXMLEngine.h>
#include <spdlog/spdlog.h>

using namespace std;

ClassImp(DataEventHeader);

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

void SimulationGeometryInfo::PopulateFromGdml(const TString& gdmlFilename) {
    spdlog::info("SimulationGeometryInfo::PopulateFromGdml");
    // Geometry must be in GDML
    TXMLEngine xml;
    XMLDocPointer_t xmldoc = xml.ParseFile(gdmlFilename.Data());
    if (!xmldoc) {
        spdlog::error("SimulationGeometryInfo::PopulateFromGdml - GDML file '{}' not found", gdmlFilename.Data());
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
        spdlog::error("SimulationGeometryInfo::PopulateFromGdml - Generated a duplicate name, please check assembly");
        exit(1);
    }
}

TString SimulationGeometryInfo::GetAlternativeNameFromGeant4PhysicalName(const TString& geant4PhysicalName) const {
    if (fGeant4PhysicalNameToNewPhysicalNameMap.count(geant4PhysicalName) > 0) {
        return fGeant4PhysicalNameToNewPhysicalNameMap.at(geant4PhysicalName);
    }
    return geant4PhysicalName;
}

Int_t SimulationGeometryInfo::GetIDFromVolumeName(const TString& volumeName) const {
    for (int i = 0; i < fGdmlNewPhysicalNames.size(); i++) {
        if (volumeName.EqualTo(fGdmlNewPhysicalNames[i])) {
            return i;
        }
    }

    for (int i = 0; i < fPhysicalVolumes.size(); i++) {
        if (volumeName.EqualTo(fPhysicalVolumes[i])) {
            return i;
        }
    }

    spdlog::error("SimulationGeometryInfo::GetIDFromPhysicalName - ID not found for '{}'", volumeName);
    exit(1);
}
