#include "models/average_model.hpp"

#include <vector>

InferenceResponse
AverageModel::infer(
    const InferenceRequest& request)
{
    const auto& data = request.input;

    double sum = 0;

    for (auto x : data)
        sum += x;

    double avg = data.empty() ? 0.0 : sum / data.size();

    return
    {
        true,
        {},
        avg,
        ""
    };
}