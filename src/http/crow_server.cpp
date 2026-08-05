#include "crow_server.hpp"
#include <nlohmann/json.hpp>

CrowServer::CrowServer(
    Router& router,
    const HttpConfig& config
) : router(router), config(config) {
    registerRouters();
}

void CrowServer::registerRouters() {
    CROW_ROUTE(app_, "/<string>")
        .methods("GET"_method, "POST"_method)([this](const crow::request& req, std::string endpoint) {
            HttpRequest http_request;
            
            if (endpoint == "health") {
                http_request.endpoint = Endpoint::Health;
            } else if (endpoint == "models") {
                http_request.endpoint = Endpoint::Models;
            } else if (endpoint == "infer") {
                http_request.endpoint = Endpoint::Infer;
            } else {
                crow::response res(404, "Not found");
                return res;
            }
            
            if (req.method == "GET"_method) {
                http_request.method = Method::Get;
            } else if (req.method == "POST"_method) {
                http_request.method = Method::Post;
            } else {
                crow::response res(405, "Method not allowed");
                return res;
            }
            
            if (http_request.endpoint == Endpoint::Infer && 
                http_request.method == Method::Post) {
                try {
                    auto body_json = crow::json::load(req.body);
                    if (body_json) {
                        InferenceRequest inference_req;
                        inference_req.model_id = body_json["model_id"].s();
                        
                        inference_req.input = req.body;
                        
                        http_request.inference_request = inference_req;
                    }
                } catch (const std::exception& e) {
                    crow::response res(400, "Invalid JSON");
                    return res;
                }
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
            } else {
                crow_res.body = response.message;
            }
            
            return crow_res;
        });
}

void CrowServer::start() {
    app_.port(config.port).multithreaded().run();
}

void CrowServer::stop() {
    app_.stop();
}