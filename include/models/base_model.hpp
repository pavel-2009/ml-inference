#pragma once

#include "imodel.hpp"
#include "model_info.hpp"

class BaseModel : public IModel
{
public:

    explicit BaseModel(ModelInfo config)
        : config_(std::move(config))
    {
    }

    void load() override
    {
        loaded_ = true;
    }

    void unload() override
    {
        loaded_ = false;
    }

    bool ready() const override
    {
        return loaded_;
    }

    const ModelInfo& config() const
    {
        return config_;
    }

protected:

    ModelInfo config_;

    bool loaded_ = false;
};