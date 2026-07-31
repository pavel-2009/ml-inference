#include "async-loader/async-loader.hpp"
#include "threadpool/task_queue.hpp"
#include "threadpool/threadpool.hpp"
#include "model/model_factory.hpp"
#include "model/model_manager.hpp"
#include "model/model_info.hpp"
#include "inference-service/inference-service.hpp"

#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

namespace fs = std::filesystem;
using json = nlohmann::json;

// Вспомогательная функция для вывода информации о модели
void printModelInfo(const ModelInfo& info, const std::string& prefix = "  ") {
    std::cout << prefix << "📋 Model Info:\n";
    std::cout << prefix << "  ID:          " << info.id << '\n';
    std::cout << prefix << "  Type:        " << info.type << '\n';
    std::cout << prefix << "  Name:        " << info.name << '\n';
    std::cout << prefix << "  Version:     " << info.version << '\n';
    std::cout << prefix << "  Author:      " << info.author << '\n';
    std::cout << prefix << "  Description: " << info.description << '\n';
    std::cout << prefix << "  Enabled:     " << (info.enabled ? "true" : "false") << '\n';
}

// Вспомогательная функция для тестирования inference
void testInference(InferenceService& service, const std::string& model_id, const std::vector<int>& input, 
                   const std::string& test_name = "") {
    if (!test_name.empty()) {
        std::cout << "\n🧪 Тест: " << test_name << "\n";
    }
    
    InferenceRequest request;
    request.model_id = model_id;
    request.input = input;
    
    std::cout << "  📤 Отправка запроса для модели '" << model_id << "'\n";
    std::cout << "  📥 Входные данные: [";
    for (size_t i = 0; i < input.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << input[i];
    }
    std::cout << "]\n";
    
    InferenceResponse response = service.infer(request);
    
    if (response.success) {
        std::cout << "  ✅ Успешно!\n";
        if (!response.result_int.empty()) {
            std::cout << "  📤 Результат (int): [";
            for (size_t i = 0; i < response.result_int.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << response.result_int[i];
            }
            std::cout << "]\n";
        } else {
            std::cout << "  📤 Результат (double): " << response.result_double << '\n';
        }
    } else {
        std::cout << "  ❌ Ошибка: " << response.error << '\n';
    }
}

int main() {
    std::cout << "=========================================\n";
    std::cout << "🚀 ТЕСТИРОВАНИЕ ASYNCLOADER\n";
    std::cout << "=========================================\n\n";
    
    // 1. Создаём зависимости
    std::cout << "📦 Создание зависимостей...\n";
    TaskQueue queue;
    ThreadPool pool(4, queue);
    ModelFactory factory;
    ModelManager manager;
    InferenceService inference_service(manager);
    std::cout << "✅ Зависимости созданы\n\n";
    
    // 2. Проверяем директорию
    fs::path models_dir = fs::current_path() / "models";
    std::cout << "📁 Директория: " << models_dir << '\n';
    
    if (!fs::exists(models_dir)) {
        std::cerr << "❌ Директория не существует!\n";
        return 1;
    }
    
    std::vector<fs::path> json_files;
    for (const auto& entry : fs::directory_iterator(models_dir)) {
        if (fs::is_regular_file(entry.path()) && entry.path().extension() == ".json") {
            json_files.push_back(entry.path());
            std::cout << "  📄 " << entry.path().filename() << '\n';
        }
    }
    std::cout << "📊 Найдено JSON файлов: " << json_files.size() << "\n\n";
    
    if (json_files.empty()) {
        std::cerr << "❌ Нет JSON файлов для загрузки!\n";
        return 1;
    }
    
    // 3. Создаём AsyncLoader
    std::cout << "🔄 Создание AsyncLoader...\n";
    AsyncLoader loader(pool, factory, manager, models_dir);
    std::cout << "✅ AsyncLoader создан\n\n";
    
    // 4. Пробуем прочитать конфиг отдельного файла
    std::cout << "📖 Тест readConfig на первом файле:\n";
    try {
        ModelInfo info = loader.readConfig(json_files[0]);
        printModelInfo(info);
        std::cout << '\n';
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка readConfig: " << e.what() << "\n\n";
    }
    
    // 5. Загружаем все модели с подробным выводом
    std::cout << "⏳ Загрузка моделей...\n";
    std::cout << "----------------------------------------\n";
    
    try {
        loader.loadAll();
        loader.wait();  // Ждем завершения всех задач загрузки
        std::cout << "✅ loadAll() выполнен\n";
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка в loadAll(): " << e.what() << '\n';
    }
    
    // ============================================
    // 7. ТЕСТИРОВАНИЕ INFERENCE SERVICE
    // ============================================
    std::cout << "=========================================\n";
    std::cout << "🧪 ТЕСТИРОВАНИЕ INFERENCE SERVICE\n";
    std::cout << "=========================================\n\n";
    
    // Получаем список загруженных моделей
    std::vector<std::string> model_ids;
    for (const auto& file : json_files) {
        try {
            ModelInfo info = loader.readConfig(file);
            if (info.enabled) {
                model_ids.push_back(info.id);
            }
        } catch (...) {
            // Пропускаем ошибки чтения
        }
    }
    
    if (model_ids.empty()) {
        std::cout << "⚠️ Нет загруженных моделей для тестирования!\n";
    } else {
        std::cout << "📊 Доступные модели для тестирования:\n";
        for (const auto& id : model_ids) {
            std::cout << "  - " << id << '\n';
        }
        std::cout << '\n';
        
        // Тестируем каждую модель
        for (const auto& model_id : model_ids) {
            std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
            
            // Тест 1 - массив целых чисел
            std::vector<int> test_input = {5, 3, 8, 1, 9, 2};
            testInference(inference_service, model_id, test_input, 
                         "Сортировка массива для " + model_id);
            
            // Тест 2 - массив с отрицательными числами
            std::vector<int> negative_input = {-5, 3, -8, 1, -9, 2};
            testInference(inference_service, model_id, negative_input,
                         "Массив с отрицательными числами для " + model_id);
            
            // Тест 3 - пустой массив (проверка обработки)
            std::vector<int> empty_array = {};
            testInference(inference_service, model_id, empty_array,
                         "Пустой массив для " + model_id);
            
            // Тест 4: Некорректная модель
            testInference(inference_service, "non_existent_model", test_input,
                         "Запрос к несуществующей модели");
            
            // Тест 5: Запрос без обязательных полей
            InferenceRequest invalid_request;
            invalid_request.model_id = "";
            invalid_request.input = {1, 2, 3};
            std::cout << "\n🧪 Тест: Запрос без model_id\n";
            InferenceResponse invalid_response = inference_service.infer(invalid_request);
            if (!invalid_response.success) {
                std::cout << "  ✅ Ошибка корректно обработана: " << invalid_response.error << '\n';
            } else {
                std::cout << "  ❌ Ошибка не была обнаружена!\n";
            }
            
            // Тест 6 - большой массив
            std::vector<int> large_array;
            for (int i = 100; i > 0; --i) {
                large_array.push_back(i);
            }
            testInference(inference_service, model_id, large_array,
                         "Большой массив (100 элементов) для " + model_id);
            
            std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
        }
    }
    
    std::cout << "\n=========================================\n";
    std::cout << "✅ Тестирование завершено!\n";
    std::cout << "=========================================\n";
    
    return 0;
}