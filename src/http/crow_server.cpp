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

void CrowServer::registerRouters() {
    CROW_ROUTE(app_, "/health")
        .methods("GET"_method)([this](const crow::request& req) {
            auto http_request = HttpRequest{
                .endpoint = Endpoint::Health,
                .method = Method::Get
            };
            
            auto response = router.route(http_request);
            
            crow::response crow_res;
            crow_res.code = response.status;
            crow_res.body = response.message;
            return crow_res;
        });

    CROW_ROUTE(app_, "/models")
        .methods("GET"_method)([this](const crow::request& req) {
            auto http_request = HttpRequest{
                .endpoint = Endpoint::Models,
                .method = Method::Get
            };
            
            auto response = router.route(http_request);
            
            crow::response crow_res;
            crow_res.code = response.status;
            crow_res.body = response.message;
            return crow_res;
        });

    CROW_ROUTE(app_, "/infer")
        .methods("POST"_method)([this](const crow::request& req) {
            InferenceRequest inference_req;
            
            try {
                auto body_json = crow::json::load(req.body);
                if (!body_json) {
                    crow::response res(400, "Invalid JSON");
                    return res;
                }
                
                inference_req.input = 
                // Add other fields as needed
                
            } catch (const std::exception& e) {
                crow::response res(400, "Failed to parse request: " + std::string(e.what()));
                return res;
            }
            
            auto http_request = HttpRequest{
                .endpoint = Endpoint::Infer,
                .method = Method::Post,
                .inference_request = inference_req
            };
            
            auto response = router.route(http_request);
            
            crow::response crow_res;
            crow_res.code = response.status;
            
            // If there's an inference response, serialize it to JSON
            if (response.inference_response.has_value()) {
                crow::json::wvalue result;
                result["generated_text"] = response.inference_response->generated_text;
                result["tokens_used"] = response.inference_response->tokens_used;
                // Add other fields as needed
                crow_res.body = result.dump();
            } else {
                crow_res.body = response.message;
            }
            
            return crow_res;
        });
}