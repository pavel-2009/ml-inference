#pragma once

#include "base_model.hpp"

class AverageModel : public BaseModel
{
public:

    using BaseModel::BaseModel;

    InferenceResponse infer(
        const InferenceRequest& request
    ) override;
};