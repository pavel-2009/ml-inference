#pragma once

#include <string>
#include <vector>
#include <any>

struct InferenceRequest
{
    std::string model_id;
    std::any input;
};