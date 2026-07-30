#pragma once

#include "base_model.hpp"

class SortModel : public BaseModel
{
public:

    using BaseModel::BaseModel;

    InferenceResponse infer(
        const InferenceRequest& request
    ) override;
};