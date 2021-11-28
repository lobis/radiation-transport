//
// Created by lobis on 22/11/2021.
//

#include <TStopwatch.h>
#include <spdlog/spdlog.h>

#include "Application.h"

using namespace std;

int main(int argc, char** argv) {
    TStopwatch timer;
    timer.Start();

    Application app(argc, argv);

    app.PrintConfig();

    app.UserInitialization();

    app.Initialize();

    spdlog::info("Time elapsed {:0.2f} seconds", timer.RealTime());

    return 0;
}