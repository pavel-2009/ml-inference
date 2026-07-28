#pragma once

#include "base_model.hpp"

#include <shared_mutex>
#include <unordered_map>


class ModelManager {
    private:
        std::unordered_map<std::string, std::shared_ptr<IModel>> models_;
        mutable std::shared_mutex mutex_;

    private:
        bool contains(const std::string& id) const;

    public:
        std::shared_ptr<IModel> get(const std::string& id);
        
        void add(const std::string& id, std::shared_ptr
            <IModel> model);

        void remove(const std::string& id);

        void clear();
};
