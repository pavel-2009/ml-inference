#pragma once

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class IModel
{
public:

    virtual ~IModel() = default;

    virtual std::string id() const = 0;

    virtual std::string name() const = 0;

    virtual std::string version() const = 0;

    virtual bool ready() const = 0;

    virtual void load() = 0;

    virtual void unload() = 0;

    virtual json infer(const json& input) = 0;
};