#include "models/sort_model.hpp"

#include <algorithm>
#include <vector>

InferenceResponse
SortModel::infer(
    const InferenceRequest& request)
{
    auto data =
        request.data.get<std::vector<int>>();

    std::sort(
        data.begin(),
        data.end());

    return
    {
        true,
        data,
        ""
    };
}