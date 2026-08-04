#pragma once

#include "inference_service/inference_service.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "model/inference_request.hpp"
#include "model/inference_response.hpp"


class Router {
    private:
        InferenceService& inference_;

    private:
        HttpResponse infer(const HttpRequest& request);

        HttpResponse health();

        HttpResponse models();

    public:
        explicit Router(InferenceService& inference);

        HttpResponse route(const HttpRequest& request);
};
