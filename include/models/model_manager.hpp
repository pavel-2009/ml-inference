#include "base_model.hpp"
#include "model_factory.hpp"

#include <mutex>


class ModelManager {
    private:
        std::unordered_map<std::string, std::shared_ptr<BaseModel>> models_;

        mutable std::mutex mutex_;

        void add(std::shared_ptr<BaseModel> model);

        bool contains(const std::string& id) const;

    public:
        std::shared_ptr<BaseModel> get(const std::string& id);

        void remove(const std::string& id);

        void clear();
};
