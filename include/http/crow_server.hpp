#include "http/crow_server.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

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
    
    // Infer endpoint
    CROW_ROUTE(app_, "/infer")
        .methods("POST"_method)([this](const crow::request& req) {
            HttpRequest http_request;
            http_request.endpoint = Endpoint::Infer;
            http_request.method = Method::Post;
            
            try {
                auto body_json = crow::json::load(req.body);
                if (!body_json) {
                    crow::response res(400, "Invalid JSON");
                    return res;
                }
                
                if (!body_json.has("model_id") || !body_json["model_id"].is_string()) {
                    crow::response res(400, "Missing or invalid 'model_id' field");
                    return res;
                }
                
                InferenceRequest inference_req;
                inference_req.model_id = body_json["model_id"].s();
                
                // Парсим input как массив целых чисел
                if (body_json.has("input")) {
                    if (body_json["input"].is_array()) {
                        std::vector<int> input;
                        for (const auto& val : body_json["input"]) {
                            if (val.is_number()) {
                                input.push_back(val.i());
                            }
                        }
                        inference_req.input = input;
                    } else {
                        crow::response res(400, "'input' must be an array of integers");
                        return res;
                    }
                } else {
                    crow::response res(400, "Missing 'input' field");
                    return res;
                }
                
                http_request.inference_request = inference_req;
                
            } catch (const std::exception& e) {
                crow::response res(400, std::string("Invalid request: ") + e.what());
                return res;
            }
            
            auto response = router.route(http_request);
            
            crow::response crow_res;
            crow_res.code = response.status;
            
            if (response.inference_response.has_value()) {
                nlohmann::json json_response;
                const auto& infer_resp = response.inference_response.value();
                json_response["success"] = infer_resp.success;
                
                if (infer_resp.success) {
                    if (!infer_resp.result_int.empty()) {
                        json_response["result"] = infer_resp.result_int;
                    } else {
                        json_response["result"] = infer_resp.result_double;
                    }
                } else {
                    json_response["error"] = infer_resp.error;
                }
                
                crow_res.body = json_response.dump();
                crow_res.set_header("Content-Type", "application/json");
            } else {
                crow_res.body = response.message;
            }
            
            return crow_res;
        });
    
    // Root endpoint - просто информация
    CROW_ROUTE(app_, "/")
        .methods("GET"_method)([]() {
            crow::response res;
            res.code = 200;
            res.body = R"({
                "service": "ML Inference Server",
                "version": "1.0.0",
                "endpoints": {
                    "GET /health": "Health check",
                    "GET /models": "List loaded models",
                    "POST /infer": "Run inference"
                }
            })";
            res.set_header("Content-Type", "application/json");
            return res;
        });
}

void CrowServer::start() {
    std::cout << "Crow server starting on port " << config.port << "\n";
    app_.port(config.port).multithreaded().run();
}

void CrowServer::stop() {
    std::cout << "Stopping Crow server...\n";
    app_.stop();
}