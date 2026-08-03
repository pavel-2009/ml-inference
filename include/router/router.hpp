#pragma once

#include "inference_service.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "inference_request.hpp"
#include "inference_response.hpp"


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
