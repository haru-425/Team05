#include "battery.h"

battery::battery()
{
    scale.x = scale.y = scale.z = 0.0169f; // スケール設定（非常に小さい）
    angle.z = DirectX::XMConvertToRadians(20.0f); 
    angle.y = DirectX::XMConvertToRadians(10.0f);  
}

battery::~battery()
{

}

void battery::Update(float elapsedTime)
{

    angle.y += RATESPEED * elapsedTime;

    //スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.x);
    //回転行列
    DirectX::XMMATRIX RX = DirectX::XMMatrixRotationX(angle.x);
    DirectX::XMMATRIX RY = DirectX::XMMatrixRotationY(angle.y);
    DirectX::XMMATRIX RZ = DirectX::XMMatrixRotationZ(angle.z);
    DirectX::XMMATRIX R = RZ * RX * RY;
    //位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);


    // 回転軸をずらすための行列（pivot = 回転中心の相対オフセット）
    DirectX::XMFLOAT3 pivot = { offset.x, offset.y, offset.z };
    //DirectX::XMFLOAT3 pivot = { offsets[0], offsets[1], offsets[2]};//デバック用
    DirectX::XMMATRIX PivotT = DirectX::XMMatrixTranslation(-pivot.x, -pivot.y, -pivot.z);  // 回転中心に移動
    DirectX::XMMATRIX PivotInvT = DirectX::XMMatrixTranslation(pivot.x, pivot.y, pivot.z);  // 回転後、元に戻す

    // ピボット考慮したワールド行列の計算
    DirectX::XMMATRIX W = S * PivotT * R * PivotInvT * T;
    //計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&world, W);

    //
    //ImGUI();
}

void battery::Render(const RenderContext& rc, ModelRenderer* renderer)
{
   // textures->Set(rc);
    renderer->Render(rc, world, model.get(), ShaderId::Lambert);
    //textures->Clear(rc);
}