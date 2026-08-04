#include "models/average_model.hpp"

#include <vector>
#include <algorithm>

InferenceResponse
AverageModel::infer(
    const std::optional<InferenceRequest>& request)
{
    if ((request->input.type() != typeid(std::vector<int>))) {
        return {
            false,
            {},
            0.0,
            "Input must be vector<int>"
        };
    }

    auto data = std::any_cast<std::vector<int>>(request->input);

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