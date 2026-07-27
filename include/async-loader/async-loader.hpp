#pragma once

#include "threadpool.hpp"
#include "model_factory.hpp"
#include "model_manager.hpp"
#include "model_info.hpp"

#include <filesystem>


class AsyncLoader {
    private:
        ThreadPool& pool_;
        ModelFactory& factory_;
        ModelManager& manager_;

    private:
        void loadModel(const std::filesystem::path& file);
        ModelInfo readConfig(const std::filesystem::path& file);

    public:
        AsyncLoader(
            ThreadPool& pool,
            ModelFactory& factory,
            ModelManager& manager,
            std::filesystem::path dir
        );

        void loadAll();
};
