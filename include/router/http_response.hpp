#pragma once

#include "inference_response.hpp"

#include <optional>
#include <string>


struct HttpResponse {
    size_t status;

    std::string message;

    std::optional<InferenceResponse> inference_response;
};