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
    if (!std::filesystem::exists(file)) {
        throw std::runtime_error("Файл не существует: " + file.string());
    }
    
    std::ifstream model_file(file);
    if (!model_file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + file.string());
    }
    
    json model_json;
    try {
        model_file >> model_json;
    } catch (const json::parse_error& e) {
        throw std::runtime_error("Ошибка парсинга JSON: " + std::string(e.what()));
    }
    
    if (!model_json.is_object()) {
        throw std::runtime_error("Корневой элемент должен быть объектом JSON");
    }
    
    std::vector<std::string> required_fields = {
        "id", "type", "name", "version", "author", "description", "enabled"
    };
    
    for (const auto& field : required_fields) {
        if (!model_json.contains(field)) {
            throw std::runtime_error("Отсутствует обязательное поле: " + field);
        }
        if (model_json[field].is_null()) {
            throw std::runtime_error("Поле " + field + " не может быть null");
        }
    }
    
    if (!model_json["id"].is_string()) {
        throw std::runtime_error("Поле id должно быть строкой");
    }
    if (!model_json["type"].is_string()) {
        throw std::runtime_error("Поле type должно быть строкой");
    }
    if (!model_json["name"].is_string()) {
        throw std::runtime_error("Поле name должно быть строкой");
    }
    if (!model_json["version"].is_string()) {
        throw std::runtime_error("Поле version должно быть строкой");
    }
    if (!model_json["author"].is_string()) {
        throw std::runtime_error("Поле author должно быть строкой");
    }
    if (!model_json["description"].is_string()) {
        throw std::runtime_error("Поле description должно быть строкой");
    }
    if (!model_json["enabled"].is_boolean()) {
        throw std::runtime_error("Поле enabled должно быть булевым");
    }
    
    ModelInfo model_info;
    model_info.id = model_json["id"].get<std::string>();
    model_info.type = model_json["type"].get<std::string>();
    model_info.name = model_json["name"].get<std::string>();
    model_info.version = model_json["version"].get<std::string>();
    model_info.author = model_json["author"].get<std::string>();
    model_info.description = model_json["description"].get<std::string>();
    model_info.enabled = model_json["enabled"].get<bool>();
    
    if (model_info.id.empty()) {
        throw std::runtime_error("Поле id не может быть пустым");
    }
    if (model_info.type.empty()) {
        throw std::runtime_error("Поле type не может быть пустым");
    }
    
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