#include "crow_server.hpp"


CrowServer::CrowServer(
    Router& router,
    const HttpConfig& config
) : router(router), config(config) {
    registerRouters();
}

void CrowServer::start() {
    app_.port(config.port).multithreaded().run();
}

void CrowServer::stop() {
    app_.stop();
}