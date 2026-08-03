#pragma once

#include "inference_request.hpp"

#include <optional>

enum class Endpoint {
    Infer,
    Health,
    Models,
};

enum class Method {
    Get,
    Post
};

struct HttpRequest {
    Endpoint endpoint;

    Method method;

    std::optional<InferenceRequest> inference_request;
};