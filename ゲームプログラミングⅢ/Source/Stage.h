#pragma once

#include"System/ModelRenderer.h"

//ステージ
class Stage
{
public:
    Stage();
    ~Stage();

    //ステージ
    void Update(float elapsedTime);

    //描画処理
    void Render(const RenderContext& rc, ModelRenderer* renderer);

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

    Model* model = nullptr;
};
