#include "async-loader.hpp"
#include "threadpool.hpp"
#include "task_queue.hpp"
#include "model_factory.hpp"
#include "model_manager.hpp"
#include "model_info.hpp"

#include <filesystem>
#include <iostream>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

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
        std::cerr << "   Создайте папку 'models' и положите в неё JSON файлы\n";
        return 1;
    }
    
    int json_count = 0;
    for (const auto& entry : fs::directory_iterator(models_dir)) {
        if (fs::is_regular_file(entry.path()) && entry.path().extension() == ".json") {
            json_count++;
            std::cout << "  📄 " << entry.path().filename() << '\n';
        }
    }
    std::cout << "📊 Найдено JSON файлов: " << json_count << "\n\n";
    
    if (json_count == 0) {
        std::cerr << "⚠️  Нет JSON файлов для загрузки!\n";
        return 1;
    }
    
    // 3. Создаём AsyncLoader
    std::cout << "🔄 Создание AsyncLoader...\n";
    AsyncLoader loader(pool, factory, manager, models_dir);
    std::cout << "✅ AsyncLoader создан\n\n";
    
    // 4. Загружаем модели
    std::cout << "⏳ Загрузка моделей...\n";
    std::cout << "----------------------------------------\n";
    loader.loadAll();
    std::cout << "----------------------------------------\n";
    
    // 5. Ждём завершения
    std::cout << "\n⏳ Ожидание завершения задач (5 сек)...\n";
    for (int i = 5; i > 0; --i) {
        std::cout << "  " << i << "...\r";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "  ✅ Готово!          \n";
    
    // 6. Итог
    std::cout << "\n=========================================\n";
    std::cout << "✅ Тестирование завершено!\n";
    std::cout << "=========================================\n";
    
    return 0;
}