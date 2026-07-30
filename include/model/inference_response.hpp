#pragma once

#include <string>
#include <vector>

struct InferenceResponse
{
    bool success = true;

    std::vector<int> result_int;
    
    double result_double = 0.0;

    std::string error;
};