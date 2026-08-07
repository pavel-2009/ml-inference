#include "router/router.hpp"
#include "async_loader/async_loader.hpp"
#include "threadpool/task_queue.hpp"
#include "threadpool/threadpool.hpp"
#include "model/model_factory.hpp"
#include "model/model_manager.hpp"
#include "inference_service/inference_service.hpp"
#include "http/crow_server.hpp"
#include "http/http_config.hpp"

#include <iostream>
#include <filesystem>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

// Глобальный флаг для graceful shutdown
std::atomic<bool> running{true};

void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down gracefully...\n";
    running = false;
}

void printBanner() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════╗
║                                                           ║
║   ML Inference Server v1.0.0                             ║
║   C++20 High-Performance Inference Engine                ║
║                                                           ║
╚═══════════════════════════════════════════════════════════╝
)";
}

void printStatus(const std::string& component, const std::string& status) {
    std::cout << "[ " << component << " ] " << status << std::endl;
}

int main(int argc, char* argv[]) {
    printBanner();
    
    // ============================================================
    // 1. Парсинг аргументов командной строки
    // ============================================================
    std::string models_dir = "models";
    uint16_t http_port = 8080;
    size_t thread_pool_size = std::thread::hardware_concurrency();
    bool enable_http = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--models-dir" && i + 1 < argc) {
            models_dir = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            http_port = static_cast<uint16_t>(std::stoi(argv[++i]));
        } else if (arg == "--threads" && i + 1 < argc) {
            thread_pool_size = static_cast<size_t>(std::stoi(argv[++i]));
        } else if (arg == "--no-http") {
            enable_http = false;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [OPTIONS]\n"
                      << "Options:\n"
                      << "  --models-dir DIR    Models directory (default: models)\n"
                      << "  --port PORT         HTTP server port (default: 8080)\n"
                      << "  --threads N         Thread pool size (default: hardware concurrency)\n"
                      << "  --no-http           Disable HTTP server\n"
                      << "  --help, -h          Show this help\n";
            return 0;
        }
    }
    
    std::cout << "Configuration:\n"
              << "  Models directory: " << models_dir << "\n"
              << "  HTTP port: " << http_port << "\n"
              << "  Thread pool size: " << thread_pool_size << "\n"
              << "  HTTP server: " << (enable_http ? "enabled" : "disabled") << "\n\n";
    
    // ============================================================
    // 2. Проверка директории с моделями
    // ============================================================
    fs::path models_path = fs::current_path() / models_dir;
    if (!fs::exists(models_path)) {
        std::cerr << "ERROR: Models directory does not exist: " << models_path << "\n";
        std::cerr << "Please create the directory and add model JSON files.\n";
        return 1;
    }
    
    // Проверяем наличие JSON файлов
    bool has_models = false;
    for (const auto& entry : fs::directory_iterator(models_path)) {
        if (fs::is_regular_file(entry.path()) && entry.path().extension() == ".json") {
            has_models = true;
            break;
        }
    }
    
    if (!has_models) {
        std::cerr << "WARNING: No JSON model files found in " << models_path << "\n";
        std::cerr << "The server will start with no models loaded.\n\n";
    }
    
    // ============================================================
    // 3. Инициализация компонентов
    // ============================================================
    printStatus("INIT", "Creating components...");
    
    // 3.1 Thread Pool и Task Queue
    TaskQueue task_queue;
    ThreadPool thread_pool(thread_pool_size, task_queue);
    printStatus("THREAD POOL", "Started with " + std::to_string(thread_pool_size) + " threads");
    
    // 3.2 Model Factory
    ModelFactory model_factory;
    printStatus("MODEL FACTORY", "Initialized");
    
    // 3.3 Model Manager
    ModelManager model_manager;
    printStatus("MODEL MANAGER", "Initialized");
    
    // 3.4 Inference Service
    InferenceService inference_service(model_manager);
    printStatus("INFERENCE SERVICE", "Initialized");
    
    // 3.5 Async Loader
    AsyncLoader async_loader(thread_pool, model_factory, model_manager, models_path);
    printStatus("ASYNC LOADER", "Initialized for: " + models_path.string());
    
    // 3.6 Router
    Router router(inference_service);
    printStatus("ROUTER", "Initialized");
    
    // 3.7 HTTP Server
    std::unique_ptr<CrowServer> http_server;
    if (enable_http) {
        HttpConfig http_config;
        http_config.port = http_port;
        http_config.threads = thread_pool_size;
        http_config.keep_alive = true;
        http_config.max_body_size = 10 * 1024 * 1024; // 10MB
        
        http_server = std::make_unique<CrowServer>(router, http_config);
        printStatus("HTTP SERVER", "Configured on port " + std::to_string(http_port));
    }
    
    // ============================================================
    // 4. Загрузка моделей
    // ============================================================
    printStatus("LOADING", "Starting asynchronous model loading...");
    std::cout << "----------------------------------------\n";
    
    try {
        async_loader.loadAll();
        async_loader.wait(); // Ждем завершения загрузки всех моделей
        std::cout << "----------------------------------------\n";
        printStatus("LOADING", "All models loaded successfully");
    } catch (const std::exception& e) {
        std::cerr << "ERROR during model loading: " << e.what() << "\n";
        std::cerr << "Server will continue with partially loaded models.\n";
    }
    
    // Выводим информацию о загруженных моделях
    auto loaded_models = model_manager.listModels();
    std::cout << "\nLoaded models: " << loaded_models.size() << "\n";
    for (const auto& model : loaded_models) {
        std::cout << "  - " << model.id << " (" << model.type << "): " 
                  << model.name << " v" << model.version << "\n";
    }
    
    // ============================================================
    // 5. Проверка работоспособности
    // ============================================================
    bool healthy = inference_service.health();
    if (healthy) {
        printStatus("HEALTH", "Service is healthy");
    } else {
        printStatus("HEALTH", "Service is unhealthy (no models loaded)");
    }
    
    // ============================================================
    // 6. Обработка сигналов для graceful shutdown
    // ============================================================
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    // ============================================================
    // 7. Запуск HTTP сервера
    // ============================================================
    if (enable_http && http_server) {
        printStatus("SERVER", "Starting HTTP server on port " + std::to_string(http_port));
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "🚀 ML Inference Server is running!\n";
        std::cout << "📡 API endpoints:\n";
        std::cout << "   GET  /health  - Health check\n";
        std::cout << "   GET  /models  - List loaded models\n";
        std::cout << "   POST /infer   - Run inference\n";
        std::cout << "\nExample inference request:\n";
        std::cout << "   curl -X POST http://localhost:" << http_port << "/infer \\\n";
        std::cout << "        -H \"Content-Type: application/json\" \\\n";
        std::cout << "        -d '{\"model_id\": \"sum\", \"input\": [1,2,3,4,5]}'\n";
        std::cout << std::string(50, '=') << "\n\n";
        
        // Запускаем сервер в отдельном потоке, чтобы можно было обрабатывать сигналы
        std::thread server_thread([&http_server]() {
            http_server->start();
        });
        
        // Ждем сигнала остановки
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Graceful shutdown
        printStatus("SERVER", "Shutting down...");
        http_server->stop();
        if (server_thread.joinable()) {
            server_thread.join();
        }
        
    } else {
        // Режим без HTTP сервера (только загрузка моделей и тесты)
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "✅ Inference server initialized (HTTP disabled)\n";
        std::cout << "Press Ctrl+C to exit\n";
        std::cout << std::string(50, '=') << "\n\n";
        
        // Демонстрация инференса
        if (!loaded_models.empty()) {
            std::cout << "Running test inference on loaded models:\n";
            for (const auto& model_info : loaded_models) {
                InferenceRequest req;
                req.model_id = model_info.id;
                req.input = std::vector<int>{1, 2, 3, 4, 5};
                
                auto response = inference_service.infer(req);
                if (response.success) {
                    std::cout << "  ✅ " << model_info.id << ": ";
                    if (!response.result_int.empty()) {
                        std::cout << "[";
                        for (size_t i = 0; i < response.result_int.size(); ++i) {
                            if (i > 0) std::cout << ", ";
                            std::cout << response.result_int[i];
                        }
                        std::cout << "]";
                    } else {
                        std::cout << response.result_double;
                    }
                    std::cout << "\n";
                } else {
                    std::cout << "  ❌ " << model_info.id << ": " << response.error << "\n";
                }
            }
        }
        
        // Ждем сигнала
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    // ============================================================
    // 8. Cleanup
    // ============================================================
    printStatus("SERVER", "Shutdown complete");
    std::cout << "Goodbye!\n";
    
    return 0;
}