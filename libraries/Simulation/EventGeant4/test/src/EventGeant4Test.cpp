//
// Created by lobis on 30/11/2021.
//

#include <TROOT.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <filesystem>

using namespace std;

namespace fs = std::filesystem;

TEST(EventGeant4, Dictionary) {
    EXPECT_TRUE(TClass::GetClass("ThisClassDoesNotExist") == nullptr);
    for (const auto& className : {"Geant4Event", "Geant4Track", "Geant4Hits", "Geant4EventHeader", "Geant4GeometryInfo"}) {
        spdlog::info("Checking existence of dictionary for: {}", className);
        EXPECT_TRUE(TClass::GetClass(className) != nullptr);
    }
}
