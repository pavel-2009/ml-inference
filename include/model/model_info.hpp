#pragma once

#include <string>

struct ModelInfo
{
    std::string id;
    std::string type;

    std::string name;
    std::string version;

    std::string author;
    std::string description;

    bool enabled = true;
};