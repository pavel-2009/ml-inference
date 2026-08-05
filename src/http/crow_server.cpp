#include "crow_server.hpp"


CrowServer::CrowServer(
    Router& router,
    const HttpConfig& config
) : router(router), config(config) {
    registerRouters();
}
