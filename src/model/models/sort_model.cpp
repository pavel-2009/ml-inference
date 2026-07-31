#include "models/sort_model.hpp"

#include <algorithm>
#include <vector>

InferenceResponse
SortModel::infer(
    const InferenceRequest& request)
{
    if ((request.input.type() != typeid(std::vector<int>))) {
        return {
            false,
            {},
            0.0,
            "Input must be vector<int>"
        };
    }

    auto data = std::any_cast<std::vector<int>>(request.input);

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