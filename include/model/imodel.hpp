#pragma once

#include "inference_request.hpp"
#include "inference_response.hpp"

class IModel
{
public:

    virtual ~IModel() = default;

    virtual void load() = 0;

    virtual void unload() = 0;

    virtual bool ready() const = 0;

    virtual InferenceResponse infer(
        const InferenceRequest& request
    ) = 0;
};