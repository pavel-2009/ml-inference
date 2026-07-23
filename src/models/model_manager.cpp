#include "models/model_manager.hpp"
#include "models/base_model.hpp"
#include "models/model_factory.hpp"

#include <iostream>


std::shared_ptr<BaseModel> ModelManager::get(const std::string& id) {
    
    if (models_.contains(id)) {
        auto model = models_.at(id);

        return model;
    } else {
        throw std::run
    }
    
}
