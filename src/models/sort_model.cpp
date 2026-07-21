#include "models/sort_model.hpp"

#include <algorithm>

SortModel::SortModel()
{
    loaded_ = false;

    info_.id = "sort";
    info_.name = "Sort";
    info_.version = "1.0";
}

std::string SortModel::id() const
{
    return info_.id;
}

std::string SortModel::name() const
{
    return info_.name;
}

std::string SortModel::version() const
{
    return info_.version;
}

bool SortModel::ready() const
{
    return loaded_;
}

void SortModel::load()
{
    loaded_ = true;
}

void SortModel::unload()
{
    loaded_ = false;
}

json SortModel::infer(const json& input)
{
    std::vector<int> data =
        input["data"].get<std::vector<int>>();

    std::sort(data.begin(), data.end());

    return
    {
        {"result", data}
    };
}