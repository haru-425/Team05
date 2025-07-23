#pragma once
#include <DirectXMath.h>
#include <vector>
#include "System/RenderContext.h"
#include "System/ShapeRenderer.h"

static enum class CollisionType
{
    AABB,
    Sphere
};

static struct AABB
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 size;
};

static struct Sphere
{
    DirectX::XMFLOAT3 centerPos;
    float radius;
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

    void Save(std::vector<AABB>& aabb);

    template<typename T>
    void Load(std::vector<T>& array);
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

    void Initialize();

    bool Collision(const DirectX::XMFLOAT3& targetPosition, float range, DirectX::XMFLOAT3& outPos); ///< �X�V����

    void Render(const RenderContext& rc, ShapeRenderer* renderer); ///< �����蔻��̉����p

    void DrawDebug(); ///< �f�o�b�O�p

private:
    void AddCollision(CollisionType type); ///< �����蔻��ǉ�

private:
    CollisionDataLoader loader; ///< �����蔻����̎擾�p
    std::vector<AABB> volumes;  ///< AABB�̏W����
    bool isVisible = true;      ///< �f�o�b�O�̉���
    CollisionType colType = CollisionType::AABB;
};