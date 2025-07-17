#include "battery.h"

battery::battery()
{
    angle.x = DirectX::XMConvertToRadians(20.0f); 
    angle.y = DirectX::XMConvertToRadians(10.0f);  
    scale.x = scale.y = scale.z = 0.000000000013f; // �X�P�[���ݒ�i���ɏ������j

}

battery::~battery()
{

}

void battery::Update(float elapsedTime)
{

    angle.z += RATESPEED * elapsedTime;

    //�X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.x);
    //��]�s��
    DirectX::XMMATRIX RX = DirectX::XMMatrixRotationX(angle.x);
    DirectX::XMMATRIX RY = DirectX::XMMatrixRotationY(angle.y);
    DirectX::XMMATRIX RZ = DirectX::XMMatrixRotationZ(angle.z);
    DirectX::XMMATRIX R = RY * RX * RZ;
    //�ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    //�R�̍s���g�ݍ��킹�A���[���h�s����쐬
    DirectX::XMMATRIX W = S * R * T;
    //�v�Z�������[���h�s������o��
    DirectX::XMStoreFloat4x4(&world, W);

}

void battery::Render(const RenderContext& rc, ModelRenderer* renderer)
{
   // textures->Set(rc);
    renderer->Render(rc, world, model.get(), ShaderId::Lambert);
    //textures->Clear(rc);
}