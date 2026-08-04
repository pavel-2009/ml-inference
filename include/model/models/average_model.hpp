#pragma once

#include "base_model.hpp"

#include <optional>

class AverageModel : public BaseModel
{
public:

    using BaseModel::BaseModel;

    InferenceResponse infer(
        const std::optional<InferenceRequest>& request
    ) override;
};