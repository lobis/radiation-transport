//
// Created by lobis on 26/11/2021.
//

#include <TApplication.h>
#include <spdlog/spdlog.h>

#include "Viewer.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc != 2) {
        spdlog::error("Application requires one single argument (root file)");
        exit(1);
    }
    auto filename = argv[1];
    spdlog::info("Input argument: {}", filename);

    TApplication app("Visualization", &argc, argv);

    Viewer viewer{};

    viewer.LoadFile(filename);
    viewer.Start();

    spdlog::info("Starting Visualization");
    app.Run();
    return 0;
}