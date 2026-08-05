#pragma once

#include <string>
#include <thread>
#include <cstdint>


class HttpConfig {
    public:
        std::string address = "0.0.0.0";
        uint16_t port = 8080;

        std::size_t threads = std::thread::hardware_concurrency();

        bool keep_alive = true;

        std::size_t max_body_size = 10 * 1024 * 1024;
        
        HttpConfig() = default;
        
        HttpConfig(const std::string& addr, uint16_t p, std::size_t thr = 0)
            : address(addr), port(p), threads(thr > 0 ? thr : std::thread::hardware_concurrency()) {}
};
