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
    std::cout << "🚀 Тестирование AsyncLoader\n";
    std::cout << "==============================\n\n";
    
    // 1. Создаём зависимости (используем реальные классы)
    TaskQueue queue;
    ThreadPool pool(4, queue);
    ModelFactory factory;      // твоя реальная фабрика
    ModelManager manager;      // твой реальный менеджер
    
    // 2. Указываем директорию с моделями
    fs::path models_dir = fs::current_path() / "models";
    
    // 3. Создаём AsyncLoader
    AsyncLoader loader(pool, factory, manager, models_dir);
    
    // 4. Загружаем все модели
    std::cout << "⏳ Загрузка моделей из: " << models_dir << '\n';
    loader.loadAll();
    
    // 5. Ждём завершения
    std::cout << "⏳ Ожидание завершения...\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    std::cout << "\n✅ Готово!\n";
    
    return 0;
}