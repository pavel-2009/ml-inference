#pragma once

#include "inference_request.hpp"
#include "inference_response.hpp"
#include "model_info.hpp"

#include <optional>

class IModel
{
public:

    virtual ~IModel() = default;

    virtual void load() = 0;

    virtual void unload() = 0;

    virtual bool ready() const = 0;

    virtual InferenceResponse infer(
        const std::optional<InferenceRequest>& request
    ) = 0;

    virtual const ModelInfo& config() const = 0;
};