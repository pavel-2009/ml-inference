#include "base_model.hpp"
#include "model_info.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <memory>
#include <stdexcept>

using nlohmann::json;

enum class ModelType
{
    AVERAGE,
    SUM,
    SORT
};

class ModelFactory {
public:
    static std::unique_ptr<BaseModel> createModel(ModelType type) {
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
    
    static std::unique_ptr<BaseModel> createModelFromFile(const std::string& file_path) {
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
        
        return std::make_unique<BaseModel>(std::move(info));
    }
};