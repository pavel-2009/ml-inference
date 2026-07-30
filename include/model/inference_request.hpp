#pragma once

#include <string>
#include <vector>

struct InferenceRequest
{
    std::string model_id;
    std::vector<int> input;
};