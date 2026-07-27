#include "async-loader.hpp"
#include "threadpool.hpp"
#include "model_factory.hpp"
#include "model_manager.hpp"
#include "model_info.hpp"
#include "imodel.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <stdexcept>
#include <iostream>

using nlohmann::json;


AsyncLoader::AsyncLoader(
    ThreadPool& pool,
    ModelFactory& factory,
    ModelManager& manager,
    std::filesystem::path dir
) : pool_(pool), factory_(factory), manager_(manager), models_dir_(dir) {}


ModelInfo AsyncLoader::readConfig(const std::filesystem::path& file) {
    ModelInfo model_info;
    json model_json;

    std::ifstream model_file(file);
    if (!model_file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + file.string());
    }

    try {
        model_file >> model_json;
    } catch (const json::parse_error& e) {
        throw std::runtime_error("Ошибка парсинга JSON: " + std::string(e.what()));
    }

    model_info.id = model_json["id"].get<std::string>();
    model_info.type = model_json["type"].get<std::string>();
    model_info.name = model_json["name"].get<std::string>();
    model_info.version = model_json["version"].get<std::string>();
    model_info.author = model_json["author"].get<std::string>();
    model_info.description = model_json["description"].get<std::string>();
    model_info.enabled = model_json["enabled"].get<bool>();

    return model_info;
}

ModelType stringToModelType(const std::string& type_str) {
    if (type_str == "average") {
        return ModelType::AVERAGE;
    } else if (type_str == "sum") {
        return ModelType::SUM;
    } else if (type_str == "sort") {
        return ModelType::SORT;
    } else {
        throw std::runtime_error("Неизвестный тип модели: " + type_str);
    }
}

void AsyncLoader::loadModel(const std::filesystem::path& file) {
    ModelInfo config = readConfig(file);
    
    if (!config.enabled) {
        std::cout << "Модель " << config.id << " отключена, пропускаем\n";
        return;
    }

    ModelType type = stringToModelType(config.type);
    
    std::shared_ptr<IModel> model;

    model = factory_.createModel(type, config);

    manager_.add(config.id, model);

    return;
};