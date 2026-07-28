#pragma once

#include "imodel.hpp"
#include "model_info.hpp"

#include <memory>
#include <string>

enum class ModelType
{
    AVERAGE,
    SUM,
    SORT
};

class ModelFactory {
public:
    static std::shared_ptr<IModel> createModel(ModelType type, ModelInfo info);
};