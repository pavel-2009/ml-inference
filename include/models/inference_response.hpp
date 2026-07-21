#pragma once

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

struct InferenceResponse
{
    bool success = true;

    json result;

    std::string error;
};