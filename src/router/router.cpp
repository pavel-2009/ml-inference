#include "router.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "inference_service.hpp"

#include <iostream>


explicit Router::Router(InferenceService& inference)
    : inference_(inference) {};


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
    };

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
