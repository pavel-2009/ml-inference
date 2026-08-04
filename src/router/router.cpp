#include "router.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "inference_service.hpp"


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
