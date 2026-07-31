#pragma once

#include "threadpool.hpp"
#include "model/model_factory.hpp"
#include "model/model_info.hpp"
#include "model/model_manager.hpp"

#include <filesystem>
#include <string>


class AsyncLoader {
    private:
        ThreadPool& pool_;
        ModelFactory& factory_;
        ModelManager& manager_;
        std::filesystem::path models_dir_;

    public:
        explicit AsyncLoader(
            ThreadPool& pool,
            ModelFactory& factory,
            ModelManager& manager,
            const std::filesystem::path& dir
        );

        ModelInfo readConfig(const std::filesystem::path& file) const;

        void loadModel(const std::filesystem::path& file);

        void loadAll();

        void wait();
};
