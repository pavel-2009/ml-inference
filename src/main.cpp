#include "router/router.hpp"
#include "async_loader/async_loader.hpp"
#include "threadpool/task_queue.hpp"
#include "threadpool/threadpool.hpp"
#include "model/model_factory.hpp"
#include "model/model_manager.hpp"
#include "model/model_info.hpp"
#include "inference_service/inference_service.hpp"
#include "http/crow_server.hpp"
#include "http/http_config.hpp"

#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <any>

namespace fs = std::filesystem;
using json = nlohmann::json;

void printModelInfo(const ModelInfo& info, const std::string& prefix = "  ") {
    std::cout << prefix << " Model Info:\n";
    std::cout << prefix << "  ID:          " << info.id << '\n';
    std::cout << prefix << "  Type:        " << info.type << '\n';
    std::cout << prefix << "  Name:        " << info.name << '\n';
    std::cout << prefix << "  Version:     " << info.version << '\n';
    std::cout << prefix << "  Author:      " << info.author << '\n';
    std::cout << prefix << "  Description: " << info.description << '\n';
    std::cout << prefix << "  Enabled:     " << (info.enabled ? "true" : "false") << '\n';
}

void testInference(InferenceService& service, const std::string& model_id, const std::vector<int>& input, 
                   const std::string& test_name = "") {
    if (!test_name.empty()) {
        std::cout << "\n Тест: " << test_name << "\n";
    }
    
    InferenceRequest request;
    request.model_id = model_id;
    request.input = input;
    
    std::cout << "   Отправка запроса для модели '" << model_id << "'\n";
    std::cout << "   Входные данные: [";
    for (size_t i = 0; i < input.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << input[i];
    }
    std::cout << "]\n";
    
    InferenceResponse response = service.infer(request);
    
    if (response.success) {
        std::cout << "  ✅ Успешно!\n";
        if (!response.result_int.empty()) {
            std::cout << "   Результат (int): [";
            for (size_t i = 0; i < response.result_int.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << response.result_int[i];
            }
            std::cout << "]\n";
        } else {
            std::cout << "   Результат (double): " << response.result_double << '\n';
        }
    } else {
        std::cout << "  ❌ Ошибка: " << response.error << '\n';
    }
}

int main() {
    std::cout << "=========================================\n";
    std::cout << " ТЕСТИРОВАНИЕ ML INFERENCE\n";
    std::cout << "=========================================\n\n";
    
    // 1. Создаём зависимости
    std::cout << " Создание зависимостей...\n";
    TaskQueue queue;
    ThreadPool pool(4, queue);
    ModelFactory factory;
    ModelManager manager;
    InferenceService inference_service(manager);
    std::cout << "✅ Зависимости созданы\n\n";
    
    // 2. Проверяем директорию
    fs::path models_dir = fs::current_path() / "models";
    std::cout << " Директория: " << models_dir << '\n';
    
    if (!fs::exists(models_dir)) {
        std::cerr << "❌ Директория не существует!\n";
        return 1;
    }
    
    std::vector<fs::path> json_files;
    for (const auto& entry : fs::directory_iterator(models_dir)) {
        if (fs::is_regular_file(entry.path()) && entry.path().extension() == ".json") {
            json_files.push_back(entry.path());
            std::cout << "   " << entry.path().filename() << '\n';
        }
    }
    std::cout << " Найдено JSON файлов: " << json_files.size() << "\n\n";
    
    if (json_files.empty()) {
        std::cerr << "❌ Нет JSON файлов для загрузки!\n";
        return 1;
    }
    
    // 3. Создаём AsyncLoader
    std::cout << " Создание AsyncLoader...\n";
    AsyncLoader loader(pool, factory, manager, models_dir);
    std::cout << "✅ AsyncLoader создан\n\n";
    
    // 4. Пробуем прочитать конфиг отдельного файла
    std::cout << " Тест readConfig на первом файле:\n";
    try {
        ModelInfo info = loader.readConfig(json_files[0]);
        printModelInfo(info);
        std::cout << '\n';
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка readConfig: " << e.what() << "\n\n";
    }
    
    // 5. Загружаем все модели
    std::cout << "⏳ Загрузка моделей...\n";
    std::cout << "----------------------------------------\n";
    
    try {
        loader.loadAll();
        loader.wait();
        std::cout << "✅ loadAll() выполнен\n";
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка в loadAll(): " << e.what() << '\n';
    }
    
    // Тестирование моделей
    std::cout << "\n=========================================\n";
    std::cout << " ТЕСТИРОВАНИЕ МОДЕЛЕЙ\n";
    std::cout << "=========================================\n\n";
    
    std::vector<std::string> model_ids = {"average", "sum", "sort"};
    std::vector<int> test_input = {5, 3, 8, 1, 9, 2};
    
    for (const auto& model_id : model_ids) {
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << " Тестирование модели: " << model_id << "\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        
        testInference(inference_service, model_id, test_input, "Обычный массив");
        testInference(inference_service, model_id, {1,2,3,4,5}, "Массив 1-5");
        testInference(inference_service, model_id, {42}, "Один элемент");
        testInference(inference_service, model_id, {}, "Пустой массив");
        
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    }
    
    // Тестирование HTTP сервера (опционально)
    std::cout << "\n=========================================\n";
    std::cout << " ЗАПУСК HTTP СЕРВЕРА (опционально)\n";
    std::cout << "=========================================\n\n";
    
    std::cout << "Хотите запустить HTTP сервер? (y/N): ";
    std::string answer;
    std::getline(std::cin, answer);
    
    if (answer == "y" || answer == "Y") {
        HttpConfig http_config;
        http_config.port = 8080;
        
        Router router(inference_service);
        CrowServer server(router, http_config);
        
        std::cout << "\n🚀 Запуск HTTP сервера на порту " << http_config.port << "...\n";
        std::cout << "Нажмите Ctrl+C для остановки\n\n";
        
        server.start();
    }
    
    std::cout << "\n=========================================\n";
    std::cout << "✅ Тестирование завершено!\n";
    std::cout << "=========================================\n";
    
    return 0;
}