#include "model_factory.hpp"
#include "average_model.hpp"
#include "sort_model.hpp"
#include "sum_model.hpp"

#include <stdexcept>


std::unique_ptr<IModel> ModelFactory::create(ModelType type, const ModelInfo& info) {
    switch (type) {
        case ModelType::AVERAGE:
            return std::make_unique<AverageModel>(info);
        case ModelType::SORT:
            return std::make_unique<SortModel>(info);
        case ModelType::SUM:
            return std::make_unique<SumModel>(info);
        default:
            throw std::runtime_error("Unknown model type: " + info.type);
    }
}