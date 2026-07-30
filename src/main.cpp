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
        std::cout << "✅ loadAll() выполнен\n";
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка в loadAll(): " << e.what() << '\n';
    }
    
    std::cout << "----------------------------------------\n";
    
    // 6. Ждём завершения задач
    std::cout << "\n⏳ Ожидание завершения задач (5 сек)...\n";
    for (int i = 5; i > 0; --i) {
        std::cout << "  " << i << "...\r";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "  ✅ Готово!          \n";
    
    std::cout << "\n=========================================\n";
    std::cout << "✅ Тестирование завершено!\n";
    std::cout << "=========================================\n";
    
    return 0;
}