//
// Created by lobis on 22/11/2021.
//

#include <spdlog/spdlog.h>

#include "Application.h"

using namespace std;

int main(int argc, char** argv) {
    Application app(argc, argv);

    app.PrintConfig();

    app.UserInitialization();

    return 0;
}