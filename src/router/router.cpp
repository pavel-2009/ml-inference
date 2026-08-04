#include "router/router.hpp"
#include "inference_service/inference_service.hpp"

#include <iostream>
#include <format>


Router::Router(InferenceService& inference)
    : inference_(inference) {}


HttpResponse Router::route(const HttpRequest& request) {
    switch (request.endpoint)
    {
    case Endpoint::Infer:
        return this->infer(request);

    case Endpoint::Health:
        return this->health();

    case Endpoint::Models:
        return this->models();
    
    default:
        return this->models();
    }
}


HttpResponse Router::infer(const HttpRequest& request) {
    HttpResponse response;

    std::optional<InferenceRequest> inference_request = request.inference_request;

    if (!inference_request) {
        response.status = 400;
        response.message = "Invalid input";
        return response;
    }

    try
    {
        InferenceResponse inference_response = inference_.infer(inference_request);

        response.status = 200;
        response.message = "Success";
        response.inference_response = inference_response;

        return response;
    }
    catch(const std::exception& e)
    {
        std::string error_msg = e.what();
        std::cerr << error_msg << '\n';

        response.status = 404;
        response.message = std::format("Error while processing: {}", error_msg);
        return response;
    }
}


HttpResponse Router::health() {
    HttpResponse response;
    
    bool healthy = inference_.health();
    
    if (healthy) {
        response.status = 200;
        response.message = "OK";
    } else {
        response.status = 503;
        response.message = "Service Unavailable";
    }
    
    return response;
}


HttpResponse Router::models() {
    HttpResponse response;
    
    try {
        auto models_list = inference_.models();
        
        response.status = 200;
        response.message = "Success";
        
        // Build JSON-like response string with model info
        std::string models_json = "[";
        bool first = true;
        for (const auto& model : models_list) {
            if (!first) {
                models_json += ",";
            }
            first = false;
            models_json += "{";
            models_json += "\"id\":\"" + model.id + "\",";
            models_json += "\"type\":\"" + model.type + "\",";
            models_json += "\"name\":\"" + model.name + "\",";
            models_json += "\"version\":\"" + model.version + "\",";
            models_json += "\"author\":\"" + model.author + "\",";
            models_json += "\"description\":\"" + model.description + "\",";
            models_json += "\"enabled\":" + std::string(model.enabled ? "true" : "false");
            models_json += "}";
        }
        models_json += "]";
        
        response.message = models_json;
        
        return response;
    }
    catch (const std::exception& e)
    {
        std::string error_msg = e.what();
        std::cerr << error_msg << '\n';
        
        response.status = 500;
        response.message = std::format("Error retrieving models: {}", error_msg);
        return response;
    }
}
