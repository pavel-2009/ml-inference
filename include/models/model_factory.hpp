#pragma once

#include "base_model.hpp"
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
    static std::unique_ptr<BaseModel> createModel(ModelType type);
    static std::unique_ptr<BaseModel> createModelFromFile(const std::string& file_path);
};