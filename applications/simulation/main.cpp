//
// Created by lobis on 22/11/2021.
//

#include "Application.h"

using namespace std;

int main(int argc, char** argv) {
    Application app(argc, argv);

    app.PrintConfig();

    app.Run();

    return 0;
}