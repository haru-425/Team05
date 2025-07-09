#pragma once

#include"System/ModelRenderer.h"
#include"System/ShapeRenderer.h"
#include "Pursuer/WayPoint.h"
#include "Pursuer/Edge.h"
#include "Pursuer/Object.h"
#include "System/LoadTextures.h"

#include <memory>
#include <wrl.h>

class Aircon
{
public:
    Aircon() {};
    Aircon(DirectX::XMFLOAT3 position);
    ~Aircon() {};

    // コピー禁止
    Aircon(const Aircon&) = delete;
    Aircon& operator=(const Aircon&) = delete;

    // ムーブ許可
    Aircon(Aircon&&) noexcept = default;
    Aircon& operator=(Aircon&&) noexcept = default;

    //ステージ
    void Update(float elapsedTime);

    //描画処理
    void Render(const RenderContext& rc, ModelRenderer* renderer);
    void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

    void SetPosition(DirectX::XMFLOAT3 position) { this->position = position; }
    DirectX::XMFLOAT3 GetPosition() { return position; }

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
