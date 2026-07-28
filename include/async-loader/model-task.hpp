#pragma once

#include "base_task.hpp"
#include "async-loader.hpp"

#include <filesystem>
#include <iostream>


class ModelLoadTask : public BaseTask {
    public:
        ModelLoadTask(AsyncLoader* loader, std::filesystem::path file)
            : loader_(loader), file_(std::move(file)) {}
        
        void execute() override {
            try {
                loader_->loadModel(file_);
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка загрузки " << file_.filename() 
                        << ": " << e.what() << '\n';
            }
        };
        
    private:
        AsyncLoader* loader_;
        std::filesystem::path file_;
};
