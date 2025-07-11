#pragma once
#include <memory>
#include <string>
#include <map>
#include "System/ModelResource.h"

class ResourceManager
{
private:
    ResourceManager() {};
    ~ResourceManager() {};

public:
    static ResourceManager& Instance()
    {
        static ResourceManager instance;
        return instance;
    }

    std::shared_ptr<ModelResource> LoadModelResource(const char* filename);

    void Clear();

private:
    using ModelMap = std::map<std::string, std::shared_ptr<ModelResource>>; // string がキーで、キーに合ったModelResource にアクセスできる

    ModelMap models_;
};

