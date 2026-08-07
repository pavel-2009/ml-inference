#pragma once

#include "router/router.hpp"
#include "http/http_config.hpp"

#include <crow.h>

class CrowServer {
    private:
        crow::SimpleApp app_;
        Router& router;
        HttpConfig config;

    private:
        void registerRouters();

    public:
        CrowServer(
            Router& router,
            const HttpConfig& config
        );

        void start();

        void stop();
};