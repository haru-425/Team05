#include "ResourceManager.h"
#include "System/Graphics.h"

std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
    auto it = models_.find(std::string(filename));

    // ���ɓǂݍ��܂�Ă����ꍇ�͓ǂݍ��ݍς݂̃��\�[�X��Ԃ�
    if (it != models_.end())// end ��
    {
        //return it->second.lock();// �����Q�Ƃɂ��ĕԂ�
        return it->second;// �����Q�Ƃɂ��ĕԂ�
    }

    {
        // �V�K�Ń��f�����\�[�X�쐬���ǂݍ���
        std::shared_ptr<ModelResource> newResource(new ModelResource());
        //std::shared_ptr<ModelResource> newResource = std::make_shared<ModelResource>();
        newResource->Load(Graphics::Instance().GetDevice(), filename);
        models_.insert({ filename, newResource });// ���[�h�������\�[�X��o�^

        return newResource;
    }
}

void ResourceManager::Clear()
{
    models_.clear();
}