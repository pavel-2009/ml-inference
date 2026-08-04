#include "inference_service/inference_service.hpp"

#include <iostream>


InferenceService::InferenceService(ModelManager& manager)
    : manager_(manager) {}


InferenceResponse InferenceService::infer(const std::optional<InferenceRequest>& request) {
    InferenceResponse response;
    
    try {
        if (request.model_id.empty()) {
            response.success = false;
            response.error = "Missing required field: model_id";
            return response;
        }

        std::string model_id = request.model_id;
        
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

        try {
            response = model->infer(request);
        } catch (const std::exception& e) {
            response.success = false;
            response.error = std::string("Inference error: ") + e.what();
            std::cerr << "Inference error: " << e.what() << std::endl;
        } catch (...) {
            response.success = false;
            response.error = "Unknown inference error";
            std::cerr << "Unknown inference error for model: " << model_id << std::endl;
        }
        
    } catch (const std::exception& e) {
        response.success = false;
        response.error = std::string("Inference error: ") + e.what();
        std::cerr << "Inference error: " << e.what() << std::endl;
    } catch (...) {
        response.success = false;
        response.error = "Unknown inference error";
        std::cerr << "Unknown inference error" << std::endl;
    }
    
    return response;
}


std::vector<ModelInfo> InferenceService::models() const {
    return manager_.listModels();
}


bool InferenceService::health() const {
    auto models_list = manager_.listModels();
    return !models_list.empty();
}