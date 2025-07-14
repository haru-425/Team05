#include "ResourceManager.h"
#include "System/Graphics.h"

std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
    auto it = models_.find(std::string(filename));

    // 既に読み込まれていた場合は読み込み済みのリソースを返す
    if (it != models_.end())// end 空
    {
        //return it->second.lock();// 強い参照にして返す
        return it->second;// 強い参照にして返す
    }

    {
        // 新規でモデルリソース作成＆読み込み
        std::shared_ptr<ModelResource> newResource(new ModelResource());
        //std::shared_ptr<ModelResource> newResource = std::make_shared<ModelResource>();
        newResource->Load(Graphics::Instance().GetDevice(), filename);
        models_.insert({ filename, newResource });// ロードしたリソースを登録

        return newResource;
    }
}

void ResourceManager::Clear()
{
    models_.clear();
}