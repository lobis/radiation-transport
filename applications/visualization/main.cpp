//
// Created by lobis on 26/11/2021.
//

#include <TApplication.h>
#include <spdlog/spdlog.h>

#include "Visualization.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc > 2) {
        spdlog::error("Application opened with bad number of arguments. Only one optional argument (root file)");
        exit(1);
    }
    TString filename = "";
    if (argc == 2) {
        filename = argv[1];
        spdlog::info("Application opened with root file as argument: '{}'", filename);
    }

    TApplication app("Visualization", &argc, argv);

    spdlog::info("Starting Visualization");

    auto visualization = new Visualization(gClient->GetRoot(), 350, 80);
    if (!filename.IsNull()) {
        visualization->OpenFile(filename);
    }

    app.Run();

    return 0;
}