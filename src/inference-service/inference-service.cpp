#include "inference-service.hpp"

#include <iostream>
#include <nlohmann/json.hpp>

using nlohmann::json;

InferenceService::InferenceService(ModelManager& manager)
    : manager_(manager) {}


InferenceResponse InferenceService::infer(const InferenceRequest& request) {
    InferenceResponse response;
    
    try {
        json data = request.data;

        if (!data.contains("model_id")) {
            response.success = false;
            response.error = "Missing required field: model_id";
            return response;
        }

        if (!data.contains("input")) {
            response.success = false;
            response.error = "Missing required field: input";
            return response;
        }

        std::string model_id = data["model_id"];
        
        std::shared_ptr<IModel> model = manager_.get(model_id);
        
        if (!model) {
            response.success = false;
            response.error = "Model not found: " + model_id;
            return response;
        }

        if (!model->ready()) {
            response.success = false;
            response.error = "Model is not ready: " + model_id;
            return response;
        }

        response = model->infer(request);
        
    } catch (const json::exception& e) {
        response.success = false;
        response.error = std::string("JSON parsing error: ") + e.what();
        std::cerr << "JSON error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        response.success = false;
        response.error = std::string("Inference error: ") + e.what();
        std::cerr << "Inference error: " << e.what() << std::endl;
    }
    
    return response;
}