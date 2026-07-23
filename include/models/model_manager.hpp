#include "base_model.hpp"

#include <shared_mutex>


class ModelManager {
    private:
        std::unordered_map<std::string, std::shared_ptr<BaseModel>> models_;
        mutable std::shared_mutex mutex_;

    private:
        bool contains(const std::string& id) const;

    public:
        std::shared_ptr<BaseModel> get(const std::string& id);
        
        void add(std::string& id, std::shared_ptr<BaseModel> model);

        void remove(const std::string& id);

        void clear();
};
