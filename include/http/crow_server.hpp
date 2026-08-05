#pragma once

#include "router/router.hpp"
#include "http/http_config.hpp"

#include <crow.h>
#include <thread>

class CrowServer {
    private:
        crow::SimpleApp app_;
        Router& router_;
        HttpConfig config_;

    private:
        void registerRouters();

    public:
        CrowServer(
            Router& router,
            const HttpConfig& config = HttpConfig()
        );

        void start();

        void stop();
        
        uint16_t getPort() const { return config_.port; }
};
