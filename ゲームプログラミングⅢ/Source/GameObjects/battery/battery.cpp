#include "battery.h"

battery::battery()
{
    scale.x = scale.y = scale.z = 0.0169f; // �X�P�[���ݒ�i���ɏ������j
    angle.z = DirectX::XMConvertToRadians(20.0f); 
    angle.y = DirectX::XMConvertToRadians(10.0f);  
}

battery::~battery()
{

}

void battery::Update(float elapsedTime)
{

    angle.y += RATESPEED * elapsedTime;

    //�X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.x);
    //��]�s��
    DirectX::XMMATRIX RX = DirectX::XMMatrixRotationX(angle.x);
    DirectX::XMMATRIX RY = DirectX::XMMatrixRotationY(angle.y);
    DirectX::XMMATRIX RZ = DirectX::XMMatrixRotationZ(angle.z);
    DirectX::XMMATRIX R = RZ * RX * RY;
    //�ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);


    // ��]�������炷���߂̍s��ipivot = ��]���S�̑��΃I�t�Z�b�g�j
    DirectX::XMFLOAT3 pivot = { offset.x, offset.y, offset.z };
    //DirectX::XMFLOAT3 pivot = { offsets[0], offsets[1], offsets[2]};//�f�o�b�N�p
    DirectX::XMMATRIX PivotT = DirectX::XMMatrixTranslation(-pivot.x, -pivot.y, -pivot.z);  // ��]���S�Ɉړ�
    DirectX::XMMATRIX PivotInvT = DirectX::XMMatrixTranslation(pivot.x, pivot.y, pivot.z);  // ��]��A���ɖ߂�

    // �s�{�b�g�l���������[���h�s��̌v�Z
    DirectX::XMMATRIX W = S * PivotT * R * PivotInvT * T;
    //�v�Z�������[���h�s������o��
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