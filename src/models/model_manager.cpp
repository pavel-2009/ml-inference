#include "models/model_manager.hpp"
#include "models/base_model.hpp"
#include "models/model_factory.hpp"

#include <iostream>
#include <mutex>


std::shared_ptr<BaseModel> ModelManager::get(const std::string& id) {

    std::shared_lock lock(mutex_);
    
    if (models_.contains(id)) {
        auto model = models_.find(id);

        return model->second;
    } else {
        throw std::runtime_error("Model not found");
    }
    
}

void ModelManager::add(const std::string& id, std::shared_ptr<BaseModel> model) {
    std::unique_lock lock(mutex_);

    models_.emplace(id, std::move(model));
}

void ModelManager::remove(const std::string& id) {
    std::unique_lock lock(mutex_);

    models_.erase(id);
}

void ModelManager::clear() {
    std::unique_lock lock(mutex_);

    models_.clear();
}

bool ModelManager::contains(const std::string& id) const {
    if (models_.contains(id)) {
        return true;
    };

    return false;
}
