#include "model_factory.hpp"
#include "average_model.hpp"
#include "sort_model.hpp"
#include "sum_model.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>

using nlohmann::json;

std::unique_ptr<BaseModel> ModelFactory::createModel(ModelType type) {
    std::string file_path;
    
    switch (type) {
        case ModelType::AVERAGE:
            file_path = "models/average.json";
            break;
        case ModelType::SORT:
            file_path = "models/sort.json";
            break;
        case ModelType::SUM:
            file_path = "models/sum.json";
            break;
        default:
            file_path = "models/average.json";
            break;
    }
    
    return createModelFromFile(file_path);
}

std::unique_ptr<BaseModel> ModelFactory::createModelFromFile(const std::string& file_path) {
    std::ifstream file(file_path);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open model config file: " + file_path);
    }
    
    json model_config;
    file >> model_config;
    
    // Парсим JSON в ModelInfo
    ModelInfo info;
    info.id = model_config.value("id", "");
    info.type = model_config.value("type", "");
    info.name = model_config.value("name", "");
    info.version = model_config.value("version", "");
    info.author = model_config.value("author", "");
    info.description = model_config.value("description", "");
    info.enabled = model_config.value("enabled", true);
    
    if (info.type == "average") {
        return std::make_unique<AverageModel>(std::move(info));
    } else if (info.type == "sort") {
        return std::make_unique<SortModel>(std::move(info));
    } else if (info.type == "sum") {
        return std::make_unique<SumModel>(std::move(info));
    } else {
        throw std::runtime_error("Unknown model type: " + info.type);
    }
}