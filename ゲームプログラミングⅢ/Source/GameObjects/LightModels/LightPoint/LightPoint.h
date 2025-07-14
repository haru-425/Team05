#pragma once

#include"System/ModelRenderer.h"
#include "Pursuer/WayPoint.h"
#include "Pursuer/Edge.h"
#include "Pursuer/Object.h"
#include "System/LoadTextures.h"

#include <memory>
#include <wrl.h>

class LightPoint
{
public:
    LightPoint() {};
    LightPoint(DirectX::XMFLOAT3 position);
    ~LightPoint() {};

    // コピー禁止
    LightPoint(const LightPoint&) = delete;
    LightPoint& operator=(const LightPoint&) = delete;

    // ムーブ許可
    LightPoint(LightPoint&&) noexcept = default;
    LightPoint& operator=(LightPoint&&) noexcept = default;

    //ステージ
    void Update(float elapsedTime);

    //描画処理
    void Render(const RenderContext& rc, ModelRenderer* renderer, Model* model, LoadTextures* texture);

    void SetPosition(DirectX::XMFLOAT3 position) { this->position = position; }

    DirectX::XMFLOAT4X4 GetWorld() const { return world; }
    Model* GetModel() { return model.get(); }

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

    //Model* model = nullptr;
    std::unique_ptr<Model> model = nullptr;

    std::unique_ptr<LoadTextures> textures;
};
