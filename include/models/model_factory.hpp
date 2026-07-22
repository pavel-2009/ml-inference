#include "base_model.hpp"
#include "model_info.hpp"

#include <nlohmann/json.hpp>
#include <fstream>

using nlohmann::json;

enum class ModelType
{
    AVERAGE,
    SUM,
    SORT
};

class ModelFactory {
    public:
        static BaseModel* createModel(ModelType type) {
            switch (type) {
                case(ModelType::AVERAGE) {
                    std::ifstream file("models/average.json")
                }
                case(ModelType::SORT) {
                    std::ifstream file("models/sort.json")
                }
                case(ModelType::SUM) {
                    std::ifstream file("models/sum.json")
                }
                default {
                    std::ifstream file("models/average.json")
                }
            }

            json model_config;
            file >> model_config;
        }

}
