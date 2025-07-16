#pragma once

#include"System/ModelRenderer.h"
#include"System/ShapeRenderer.h"
#include "System/LoadTextures.h"

#include <memory>
#include <wrl.h>


class TV
{
public:
    // インスタンス取得
    static TV& Instance()
    {
        static TV instance;
        return instance;
    }

    TV() {}
    ~TV() {}

    void Initialize();
    //ステージ
    void Update(float elapsedTime);

    //描画処理
    void Render(const RenderContext& rc, ModelRenderer* renderer);
    void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

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

    std::unique_ptr<Model> model[2] = {};

    std::unique_ptr<LoadTextures> textures[2] = {};
};

