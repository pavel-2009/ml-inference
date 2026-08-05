#pragma once

#include <string>
#include <thread>
#include <cstdint>

/**
 * @brief Конфигурация HTTP сервера
 * 
 * Содержит параметры для настройки HTTP сервера Crow:
 * - адрес и порт для прослушивания
 * - количество потоков для обработки запросов
 * - настройки keep-alive соединений
 * - максимальный размер тела запроса
 */
class HttpConfig {
    public:
        /** @brief Адрес для прослушивания (по умолчанию "0.0.0.0") */
        std::string address = "0.0.0.0";
        
        /** @brief Порт для прослушивания (по умолчанию 8080) */
        uint16_t port = 8080;

        /** @brief Количество потоков для обработки запросов */
        std::size_t threads = std::thread::hardware_concurrency();

        /** @brief Включить keep-alive соединения (по умолчанию true) */
        bool keep_alive = true;

        /** @brief Максимальный размер тела запроса в байтах (по умолчанию 10MB) */
        std::size_t max_body_size = 10 * 1024 * 1024;
        
        /** @brief Конструктор по умолчанию */
        HttpConfig() = default;
        
        /**
         * @brief Конструктор с параметрами
         * @param addr Адрес для прослушивания
         * @param p Порт для прослушивания
         * @param thr Количество потоков (0 = аппаратное значение)
         */
        HttpConfig(const std::string& addr, uint16_t p, std::size_t thr = 0)
            : address(addr)
            , port(p)
            , threads(thr > 0 ? thr : std::thread::hardware_concurrency())
            , keep_alive(true)
            , max_body_size(10 * 1024 * 1024) {}
};
