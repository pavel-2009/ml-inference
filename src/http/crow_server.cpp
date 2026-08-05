#include "http/crow_server.hpp"

#include <iostream>
#include <atomic>

static std::atomic<bool> g_server_running{false};

CrowServer::CrowServer(
    Router& router,
    const HttpConfig& config
) : router_(router)
  , config_(config) {
    this->registerRouters();
}

void CrowServer::registerRouters() {
    // Health endpoint - проверка работоспособности сервиса
    CROW_ROUTE(app_, "/health")
        .methods(crow::HTTPMethod::GET)
        ([this](const crow::request&) {
            HttpRequest request;
            request.endpoint = Endpoint::Health;
            request.method = Method::Get;
            
            HttpResponse response = router_.route(request);
            
            return crow::response(response.status, response.message);
        });

    // Models endpoint - получение списка доступных моделей
    CROW_ROUTE(app_, "/models")
        .methods(crow::HTTPMethod::GET)
        ([this](const crow::request&) {
            HttpRequest request;
            request.endpoint = Endpoint::Models;
            request.method = Method::Get;
            
            HttpResponse response = router_.route(request);
            
            return crow::response(response.status, response.message);
        });

    // Infer endpoint - выполнение инференса модели
    CROW_ROUTE(app_, "/infer")
        .methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) {
            HttpRequest request;
            request.endpoint = Endpoint::Infer;
            request.method = Method::Post;
            
            try {
                auto body = crow::json::load(req.body);
                if (!body) {
                    HttpResponse response;
                    response.status = 400;
                    response.message = "Invalid JSON";
                    return crow::response(response.status, response.message);
                }
                
                InferenceRequest inf_req;
                inf_req.model_id = body["model_id"].std::string_get();
                
                if (body.find("input") != body.end()) {
                    auto input_array = body["input"].list();
                    for (const auto& item : input_array) {
                        inf_req.input.push_back(item.i());
                    }
                }
                
                request.inference_request = inf_req;
            } catch (const std::exception& e) {
                HttpResponse response;
                response.status = 400;
                response.message = std::string("Error parsing request: ") + e.what();
                return crow::response(response.status, response.message);
            }
            
            HttpResponse response = router_.route(request);
            
            if (response.inference_response) {
                crow::json::wvalue result;
                result["success"] = response.inference_response->success;
                
                if (!response.inference_response->result_int.empty()) {
                    std::vector<int> result_vec(
                        response.inference_response->result_int.begin(), 
                        response.inference_response->result_int.end()
                    );
                    result["result"] = result_vec;
                } else {
                    result["result"] = response.inference_response->result_double;
                }
                
                if (!response.inference_response->error.empty()) {
                    result["error"] = response.inference_response->error;
                }
                
                return crow::response(response.status, result.dump());
            }
            
            return crow::response(response.status, response.message);
        });
}

void CrowServer::start() {
    g_server_running = true;
    server_future_ = std::async(std::launch::async, [this]() {
        app_.port(config_.port).multithreaded().run();
    });
}

void CrowServer::stop() {
    g_server_running = false;
    app_.stop();
    if (server_future_.valid()) {
        server_future_.wait();
    }
}

bool CrowServer::isRunning() const {
    return g_server_running.load();
}
