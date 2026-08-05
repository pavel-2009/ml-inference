#include "crow_server.hpp"
#include <nlohmann/json.hpp> // or use crow's JSON if preferred

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

    // Models endpoint
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

    // Infer endpoint (POST)
    CROW_ROUTE(app_, "/infer")
        .methods("POST"_method)([this](const crow::request& req) {
            // Parse JSON body
            try {
                auto body_json = crow::json::load(req.body);
                if (!body_json) {
                    crow::response res(400, "Invalid JSON");
                    return res;
                }
                
                // Extract model_id
                std::string model_id = body_json["model_id"].s();
                
                // Extract input - could be various types
                // This is a simplified example - you'd need to handle different input types
                InferenceRequest inference_req;
                inference_req.model_id = model_id;
                
                if (body_json.has("input")) {
                    auto input_json = body_json["input"];
                    
                    if (input_json.t() == crow::json::type::Array) {
                        std::vector<int> int_vec;
                        for (const auto& val : input_json) {
                            if (val.t() == crow::json::type::Number) {
                                int_vec.push_back(val.i());
                            }
                        }
                        inference_req.input = int_vec;
                    }
                    else if (input_json.t() == crow::json::type::Number) {
                        if (input_json.d() == static_cast<double>(input_json.i())) {
                            inference_req.input = input_json.i();
                        } else {
                            inference_req.input = input_json.d();
                        }
                    }

                    else if (input_json.t() == crow::json::type::String) {
                        inference_req.input = input_json.s();
                    }
                }
                
                auto http_request = HttpRequest{
                    .endpoint = Endpoint::Infer,
                    .method = Method::Post,
                    .inference_request = inference_req
                };
                
                auto response = router.route(http_request);
                
                crow::response crow_res;
                crow_res.code = response.status;
                
                crow::json::wvalue result;
                if (response.inference_response.has_value()) {
                    const auto& infer_resp = response.inference_response.value();
                    result["success"] = infer_resp.success;
                    
                    if (infer_resp.success) {

                        if (!infer_resp.result_int.empty()) {
                            std::vector<int> result_vec = infer_resp.result_int;

                            crow::json::wvalue::list result_list;
                            for (int val : result_vec) {
                                result_list.push_back(val);
                            }
                            result["result"] = std::move(result_list);
                        } 
                        else if (infer_resp.result_double != 0.0) {
                            result["result"] = infer_resp.result_double;
                        }
                    } else {
                        result["error"] = infer_resp.error;
                    }
                } else {
                    result["message"] = response.message;
                }
                
                crow_res.body = result.dump();
                return crow_res;
                
            } catch (const std::exception& e) {
                crow::response res(500, "Internal server error: " + std::string(e.what()));
                return res;
            }
        });
}

void CrowServer::start() {
    app_.port(config.port).multithreaded().run();
}

void CrowServer::stop() {
    app_.stop();
}