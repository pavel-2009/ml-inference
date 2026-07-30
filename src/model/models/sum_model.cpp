#include "models/sum_model.hpp"

#include <vector>

InferenceResponse
SumModel::infer(
    const InferenceRequest& request)
{
    auto data =
        request.data.get<std::vector<int>>();

    int sum = 0;

    for (auto x : data)
        sum += x;

    return
    {
        true,
        sum,
        ""
    };
}