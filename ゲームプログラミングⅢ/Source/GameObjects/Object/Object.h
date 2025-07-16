#pragma once
#include"System/ModelRenderer.h"
#include"System/ShapeRenderer.h"
#include "System/LoadTextures.h"

#include <memory>
#include <wrl.h>

class Object
{
public:
    Object() {}
    Object(DirectX::XMFLOAT3 position,DirectX::XMFLOAT3 angle);
    ~Object() {}

    // コピー禁止
    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;

    // ムーブ許可
    Object(Object&&) noexcept = default;
    Object& operator=(Object&&) noexcept = default;

    //ステージ
    void Update(float elapsedTime);

    void Render(const RenderContext& rc, ModelRenderer* renderer, Model* model, LoadTextures* textures);

    //描画処理
    void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);
    void DebugGUI();

    DirectX::XMFLOAT3 GetPosition() { return position; }
    DirectX::XMFLOAT3 GetAngle() { return angle; }
    void SetPosition(DirectX::XMFLOAT3 position) { this->position = position; }
    void SetAngle(DirectX::XMFLOAT3 angle) { this->angle = angle; }

private:
    DirectX::XMFLOAT3 position = { 0,0,0 };
    DirectX::XMFLOAT3 angle = { 0,0,0 };
    DirectX::XMFLOAT3 scale = { 1,1,1 };
    DirectX::XMFLOAT4X4 world = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };

    std::unique_ptr<Model> model = nullptr;

    std::unique_ptr<LoadTextures> textures;
};

