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
