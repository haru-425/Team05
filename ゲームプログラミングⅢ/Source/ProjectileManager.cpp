#include "ProjectileManager.h"

ProjectileManager::ProjectileManager()
{
}

ProjectileManager::~ProjectileManager()
{
    Clear();
}

void ProjectileManager::Update(float elapsedTime)
{
    for (Projectile* projectile : projectiles)
    {
        projectile->Update(elapsedTime);
    }

    //�j������
    //��projectile�͈̔�for������erase()����ƕs����������Ă��܂����߁A
    //�@�X�V�������I�������ɔj�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����
    for (Projectile* projectile : removes)
    {
        //std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
        std::vector<Projectile*>::iterator it = std::find(projectiles.begin(), projectiles.end(), projectile);

        if (it != projectiles.end())
        {
            projectiles.erase(it);
        }

        //�e�ۂ̔j������
        delete projectile;
    }
    //�j�����X�g���N���A
    removes.clear();
}

void ProjectileManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    for (Projectile* projectile : projectiles)
    {
        projectile->Render(rc,renderer);
    }
}

void ProjectileManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
    for (Projectile* projectile : projectiles)
    {
        projectile->RenderDebugPrimitive(rc, renderer);
    }
}

//�v�f������
void ProjectileManager::Register(Projectile* projectile)
{
    projectiles.emplace_back(projectile);
}

void ProjectileManager::Clear()
{
    for (Projectile* projectile : projectiles)
    {
        delete projectile;
    }
    projectiles.clear();
}

void ProjectileManager::Remove(Projectile* projectile)
{
    //�j�����X�g�ɒǉ�
    removes.insert(projectile);
}
