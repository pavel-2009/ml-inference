#pragma once

#include "base_model.hpp"

class SumModel : public BaseModel
{
public:

    using BaseModel::BaseModel;

    InferenceResponse infer(
        const std::optional<InferenceRequest>& request
    ) override;
};