#pragma once
#include <DirectXMath.h>
#include <vector>
#include "System/RenderContext.h"
#include "System/ShapeRenderer.h"

static struct AABB
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 size;
};

/**
* @class CollisionEditor
* @brief �����蔻���ǂݍ���
*/
class CollisionDataLoader
{
public:
    CollisionDataLoader();
    ~CollisionDataLoader();
};

/**
* @class CollisionEditor
* @brief �}�b�v�ɓ����蔻���ݒu���邽�߂̃N���X
*/
class CollisionEditor
{
private:
    CollisionEditor() = default;
    ~CollisionEditor() = default;

public:
    static CollisionEditor& Instance()
    {
        static CollisionEditor instance;
        return instance;
    }

    void Update(float dt);

    void Render(const RenderContext& rc, ShapeRenderer* renderer);

    void AddCollision();

    void DrawDebug();

private:

private:
    CollisionDataLoader loader;
    std::vector<AABB> volumes;
    bool isVisible = true;
};