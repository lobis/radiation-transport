//
// Created by lobis on 22/11/2021.
//

#include <spdlog/spdlog.h>

#include "Application.h"

using namespace std;

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%T][%^%l%$][thread %t]: %v");

    Application app(argc, argv);

    app.PrintConfig();

    app.Run();

    return 0;
}