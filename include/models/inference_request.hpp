#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct InferenceRequest
{
    json data;
};