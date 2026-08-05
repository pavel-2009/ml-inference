#pragma once

#include <string>
#include <thread>


class HttpConfig {
    public:
        std::string address = "0.0.0.0";
        uint16_t port = 8080;

        std::size_t threads = std::thread::hardware_concurrency();

        bool keep_alive = true;

        std::size_t max_body_size = 10 * 1024 * 1024;
};
