#pragma once

#include "model/model_manager.hpp"
#include "model/inference_response.hpp"
#include "model/inference_request.hpp"
#include "model/model_info.hpp"

#include <vector>


class InferenceService {
    private:
        ModelManager& manager_;

    public:
        explicit InferenceService(ModelManager& manager);

        InferenceResponse infer(const std::optional<InferenceRequest>& request);

        std::vector<ModelInfo> models() const;

        bool health() const;
};
