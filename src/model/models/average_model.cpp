#include "models/average_model.hpp"

#include <vector>

InferenceResponse
AverageModel::infer(
    const InferenceRequest& request)
{
    auto data =
        request.data.get<std::vector<int>>();

    double sum = 0;

    for (auto x : data)
        sum += x;

    return
    {
        true,
        sum / data.size(),
        ""
    };
}