//
// Created by lobis on 25/11/2021.
//

#include "SerializableConfig.h"

#include <stdio.h>

using namespace std;

void SerializableConfig::Print() const { cout << Serialize() << endl; }

string SerializableConfig::GetAbsolutePath(const string& input) {
    if (input.empty()) {
        return "";
    }

    if (input.front() == '/') {
        return input;
    }

    return string(get_current_dir_name()) + "/" + input;
}

string SerializableConfig::GetAbsolutePathFromConfig(const string& input, const string& configFilepath) {
    if (input.empty()) {
        return "";
    }

    if (input.front() == '/') {
        return input;
    }

    size_t slash = configFilepath.find_last_of('/');
    string parentPath = (slash != std::string::npos) ? configFilepath.substr(0, slash) : configFilepath;

    return parentPath + "/" + input;
}
