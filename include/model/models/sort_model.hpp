#pragma once

#include "base_model.hpp"

#include <optional>

class SortModel : public BaseModel
{
public:

    using BaseModel::BaseModel;

    InferenceResponse infer(
        const std::optional<InferenceRequest>& request
    ) override;
};