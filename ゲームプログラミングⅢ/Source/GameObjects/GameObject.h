#pragma once
#include "System/RenderContext.h"
#include "System/ModelRenderer.h"
#include "System/ShapeRenderer.h"

// デバッグ描画モードの選択
static enum DEBUG_MODE
{
    BOX         = 1 << 0,
    CAPSULE     = 1 << 1,
    CYLINDER    = 1 << 2,
    SPHERE      = 1 << 3,
};

inline DEBUG_MODE operator|(DEBUG_MODE a, DEBUG_MODE b)
{
    return static_cast<DEBUG_MODE>(static_cast<int>(a) | static_cast<int>(b));
}

class GameObject
{
public:
    GameObject() = default;
    virtual ~GameObject() = default;

    // 更新処理
    virtual void Update(float dt) = 0;

    // 描画処理
    virtual void Render(const RenderContext& rc, ModelRenderer* renderer) = 0;

    // デバッグ描画
    void RenderDebug(const RenderContext& rc, ShapeRenderer* renderer, DirectX::XMFLOAT3 size, DirectX::XMFLOAT4 color, DEBUG_MODE debugMode)
    {
        if (debugMode & DEBUG_MODE::BOX)
            renderer->RenderBox(rc, position, angle, size, color);
        if (debugMode & DEBUG_MODE::CAPSULE)
            renderer->RenderCapsule(rc, world, radius, size.y, color);
        if (debugMode & DEBUG_MODE::CYLINDER)
            renderer->RenderCylinder(rc, position, radius, size.y, color);
        if (debugMode & DEBUG_MODE::SPHERE)
            renderer->RenderSphere(rc, position, radius, color);
    }

    // ImGui描画処理
    virtual void DrawDebug() {};

    // 行列更新処理
    void UpdateTransform()
    {
        //スケール行列を作成
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.x);
        //回転行列
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
        //位置行列を作成
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
        //３つの行列を組み合わせ、ワールド行列を作成
        DirectX::XMMATRIX W = S * R * T;
        //計算したワールド行列を取り出す
        DirectX::XMStoreFloat4x4(&world, W);
    }

    // ゲッター
    DirectX::XMFLOAT3 GetPosition() const { return position; }
    DirectX::XMFLOAT3 GetAngle() const { return angle; }
    DirectX::XMFLOAT3 GetScale() const { return scale; }
    DirectX::XMFLOAT4X4 GetWorld() const { return world; }

    float GetViewPoint() const { return viewPoint; }

    float GetRadius() const { return radius; }

    // セッター
    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
    void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

    void SetIsHit(bool isHit) { this->isHit = isHit; }

protected:
    DirectX::XMFLOAT3 position = { 0,0,0 };
    DirectX::XMFLOAT3 angle = { 0,0,0 };
    DirectX::XMFLOAT3 scale = { 1,1,1 };
    DirectX::XMFLOAT4X4 world = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };

    float viewPoint = 0;

    DirectX::XMFLOAT2 velocity = {};    // 今回は落下がないからfloat2型
    float radius = 0;                   // 探索に使う？
    float acceleration = 0;             // 加速度
    DirectX::XMFLOAT3 saveDirection;    // 最後のカメラの方向を保存するため

    bool isHit = false;
};

