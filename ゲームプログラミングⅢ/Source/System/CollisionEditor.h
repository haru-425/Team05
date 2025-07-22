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
* @brief 当たり判定を読み込む
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
* @brief マップに当たり判定を設置するためのクラス
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

    void Update(float dt); ///< 更新処理

    void Render(const RenderContext& rc, ShapeRenderer* renderer); ///< 当たり判定の可視化用

    void DrawDebug(); ///< デバッグ用

private:
    void AddCollision(CollisionType type); ///< 当たり判定追加

private:
    CollisionDataLoader loader; ///< 当たり判定情報の取得用
    std::vector<AABB> volumes;
    bool isVisible = true;
    CollisionType colType = CollisionType::AABB;
};