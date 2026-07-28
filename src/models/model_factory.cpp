#include "model_factory.hpp"
#include "average_model.hpp"
#include "sort_model.hpp"
#include "sum_model.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>

using nlohmann::json;

std::shared_ptr<IModel> ModelFactory::createModel(ModelType type, ModelInfo info) {
    std::string file_path;
    
    switch (type) {
        case ModelType::AVERAGE:
            return std::make_shared<AverageModel>(std::move(info));
        case ModelType::SORT:
            return std::make_shared<SortModel>(std::move(info));
        case ModelType::SUM:
            return std::make_shared<SumModel>(std::move(info));
        default:
            throw std::runtime_error("Unknown model type: " + info.type);
    }
}