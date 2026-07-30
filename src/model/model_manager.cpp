#include "model/model_manager.hpp"

#include <stdexcept>
#include <mutex>


std::shared_ptr<IModel> ModelManager::get(const std::string& id) const {
    std::shared_lock lock(mutex_);
    
    auto it = models_.find(id);
    if (it != models_.end()) {
        return it->second;
    }
    
    throw std::runtime_error("Model not found: " + id);
}

bool ModelManager::contains(const std::string& id) const {
    std::shared_lock lock(mutex_);
    return models_.contains(id);
}

void ModelManager::add(const std::string& id, std::shared_ptr<IModel> model) {
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
