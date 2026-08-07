#include "http/crow_server.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>

CrowServer::CrowServer(
    Router& router,
    const HttpConfig& config
) : router(router), config(config) {
    registerRouters();
}

void CrowServer::registerRouters() {
    // Health endpoint
    CROW_ROUTE(app_, "/health")
        .methods("GET"_method)([this](const crow::request& req) {
            HttpRequest http_request;
            http_request.endpoint = Endpoint::Health;
            http_request.method = Method::Get;
            
            auto response = router.route(http_request);
            
            crow::response crow_res;
            crow_res.code = response.status;
            crow_res.body = response.message;
            return crow_res;
        });
    
    // Models endpoint
    CROW_ROUTE(app_, "/models")
        .methods("GET"_method)([this](const crow::request& req) {
            HttpRequest http_request;
            http_request.endpoint = Endpoint::Models;
            http_request.method = Method::Get;
            
            auto response = router.route(http_request);
            
            crow::response crow_res;
            crow_res.code = response.status;
            crow_res.body = response.message;
            crow_res.set_header("Content-Type", "application/json");
            return crow_res;
        });
    
    // Infer endpoint - упрощенная версия
    // Infer endpoint
    // Infer endpoint
    CROW_ROUTE(app_, "/infer")
        .methods("POST"_method)([this](const crow::request& req) {
            try {
                auto body = crow::json::load(req.body);
                
                if (!body) {
                    return crow::response(400, "Invalid JSON");
                }
                
                // Получаем model_id
                std::string model_id = body["model_id"].s();
                if (model_id.empty()) {
                    return crow::response(400, "model_id is required");
                }
                
                // Получаем input как массив int
                auto input = body["input"];
                
                // Создаем запрос
                InferenceRequest inference_req;
                inference_req.model_id = model_id;
                inference_req.input = input;
                
                HttpRequest http_request;
                http_request.endpoint = Endpoint::Infer;
                http_request.method = Method::Post;
                http_request.inference_request = inference_req;
                
                // Выполняем инференс
                HttpResponse response = router.route(http_request);
                
                // Формируем ответ
                nlohmann::json result;
                result["success"] = response.status == 200;
                
                if (response.inference_response.has_value()) {
                    auto& infer = response.inference_response.value();
                    if (infer.success) {
                        if (!infer.result_int.empty()) {
                            result["result"] = infer.result_int;
                        } else {
                            result["result"] = infer.result_double;
                        }
                    } else {
                        result["error"] = infer.error;
                    }
                } else {
                    result["message"] = response.message;
                }
                
                crow::response crow_res;
                crow_res.code = response.status;
                crow_res.body = result.dump();
                crow_res.set_header("Content-Type", "application/json");
                return crow_res;
                
            } catch (const std::exception& e) {
                return crow::response(400, std::string("Error: ") + e.what());
            }
        });
    
    // Root endpoint
    CROW_ROUTE(app_, "/")
        .methods("GET"_method)([]() {
            crow::response res;
            res.code = 200;
            res.body = R"({
                "service": "ML Inference Server",
                "version": "1.0.0",
                "endpoints": {
                    "GET /health": "Health check",
                    "GET /models": "List models",
                    "POST /infer": "Run inference"
                }
            })";
            res.set_header("Content-Type", "application/json");
            return res;
        });
}

void CrowServer::start() {
    std::cout << "Starting server on port " << config.port << "\n";
    app_.port(config.port).multithreaded().run();
}

void CrowServer::stop() {
    std::cout << "Stopping server...\n";
    app_.stop();
}