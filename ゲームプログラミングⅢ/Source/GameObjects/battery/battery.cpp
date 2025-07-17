#include "battery.h"

battery::battery()
{
    angle.x = DirectX::XMConvertToRadians(20.0f); 
    angle.y = DirectX::XMConvertToRadians(10.0f);  
    scale.x = scale.y = scale.z = 0.000000000013f; // スケール設定（非常に小さい）

}

battery::~battery()
{

}

void battery::Update(float elapsedTime)
{

    angle.z += RATESPEED * elapsedTime;

    //スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.x);
    //回転行列
    DirectX::XMMATRIX RX = DirectX::XMMatrixRotationX(angle.x);
    DirectX::XMMATRIX RY = DirectX::XMMatrixRotationY(angle.y);
    DirectX::XMMATRIX RZ = DirectX::XMMatrixRotationZ(angle.z);
    DirectX::XMMATRIX R = RY * RX * RZ;
    //位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    //３つの行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;
    //計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&world, W);

}

void battery::Render(const RenderContext& rc, ModelRenderer* renderer)
{
   // textures->Set(rc);
    renderer->Render(rc, world, model.get(), ShaderId::Lambert);
    //textures->Clear(rc);
}