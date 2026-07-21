#pragma once

#include "imodel.hpp"
#include "model_info.hpp"

class SortModel : public IModel
{
public:

    SortModel();

    std::string id() const override;

    std::string name() const override;

    std::string version() const override;

    bool ready() const override;

    void load() override;

    void unload() override;

    json infer(const json&) override;

private:

    bool loaded_;

    ModelInfo info_;
};