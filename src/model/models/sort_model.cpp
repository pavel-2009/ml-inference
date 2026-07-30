#include "models/sort_model.hpp"

#include <algorithm>
#include <vector>

InferenceResponse
SortModel::infer(
    const InferenceRequest& request)
{
    auto data = request.input;

    std::sort(
        data.begin(),
        data.end());

    return
    {
        true,
        data,
        0.0,
        ""
    };
}