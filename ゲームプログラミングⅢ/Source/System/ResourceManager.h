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
    using ModelMap = std::map<std::string, std::shared_ptr<ModelResource>>; // string ���L�[�ŁA�L�[�ɍ�����ModelResource �ɃA�N�Z�X�ł���

    ModelMap models_;
};

