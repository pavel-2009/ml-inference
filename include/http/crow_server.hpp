#pragma once

#include "router.hpp"
#include "http_config.hpp"

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
