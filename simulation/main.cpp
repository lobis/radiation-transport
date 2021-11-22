//
// Created by lobis on 22/11/2021.
//

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include "TestClass.h"

int main() {
    spdlog::info("This is the simulation application!");

    TestClass test;

    spdlog::info(test.i);

    return 0;
}