#pragma once

#include "router/router.hpp"
#include "http/http_config.hpp"

#include <crow.h>
#include <thread>
#include <future>

/**
 * @brief HTTP сервер на основе библиотеки Crow
 * 
 * Обеспечивает обработку HTTP запросов и маршрутизацию через Router:
 * - GET /health - проверка работоспособности сервиса
 * - GET /models - получение списка доступных моделей
 * - POST /infer - выполнение инференса модели
 */
class CrowServer {
    private:
        crow::SimpleApp app_;
        Router& router_;
        HttpConfig config_;
        std::future<void> server_future_;

    private:
        /**
         * @brief Регистрация всех маршрутов (роутов)
         * 
         * Регистрирует обработчики для endpoint'ов:
         * - /health (GET)
         * - /models (GET)  
         * - /infer (POST)
         */
        void registerRouters();

    public:
        /**
         * @brief Конструктор сервера
         * @param router Ссылка на Router для маршрутизации запросов
         * @param config Конфигурация HTTP сервера
         */
        CrowServer(
            Router& router,
            const HttpConfig& config = HttpConfig()
        );

        /**
         * @brief Запуск сервера в отдельном потоке
         * 
         * Сервер запускается асинхронно и не блокирует основной поток.
         */
        void start();

        /**
         * @brief Остановка сервера
         */
        void stop();
        
        /**
         * @brief Получить порт сервера
         * @return Номер порта
         */
        uint16_t getPort() const { return config_.port; }
        
        /**
         * @brief Проверка работы сервера
         * @return true если сервер запущен
         */
        bool isRunning() const;
};
