#include "async-loader.hpp"

#include "threadpool.hpp"
#include "model_factory.hpp"
#include "model_manager.hpp"
#include "model_info.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

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

    model_file >> model_json;

    model_info.id = model_json["id"].get<std::string>();
    model_info.type = model_json["type"].get<std::string>();
    model_info.name = model_json["name"].get<std::string>();
    model_info.version = model_json["version"].get<std::string>();
    model_info.author = model_json["author"].get<std::string>();
    model_info.description = model_json["description"].get<std::string>();
    model_info.enabled = model_json["enabled"].get<bool>();

    return model_info;
};
