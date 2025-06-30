#pragma once

#include"System/ModelRenderer.h"

//�X�e�[�W
class Stage
{
public:
    Stage();
    ~Stage();

    //�X�e�[�W
    void Update(float elapsedTime);

    //�`�揈��
    void Render(const RenderContext& rc, ModelRenderer* renderer);

    DirectX::XMFLOAT4X4 GetWorld() const { return world; }

    Model* GetModel() const { return model; }

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
