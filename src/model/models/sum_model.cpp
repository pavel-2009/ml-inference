#include "models/sum_model.hpp"

#include <vector>

InferenceResponse
SumModel::infer(
    const InferenceRequest& request)
{
    const auto& data = request.input;

    double sum = 0;

    for (auto x : data)
        sum += x;

    return
    {
        true,
        {},
        sum,
        ""
    };
}