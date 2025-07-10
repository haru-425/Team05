#pragma once

#include"System/ModelRenderer.h"
#include "Pursuer/WayPoint.h"
#include "Pursuer/Edge.h"
#include "Pursuer/Object.h"
#include "System/LoadTextures.h"

#include <memory>
#include <wrl.h>

class LightTorus
{
public:
    LightTorus() {};
    LightTorus(DirectX::XMFLOAT3 position);
    ~LightTorus() {};

    // �R�s�[�֎~
    LightTorus(const LightTorus&) = delete;
    LightTorus& operator=(const LightTorus&) = delete;

    // ���[�u����
    LightTorus(LightTorus&&) noexcept = default;
    LightTorus& operator=(LightTorus&&) noexcept = default;

    //�X�e�[�W
    void Update(float elapsedTime);

    //�`�揈��
    void Render(const RenderContext& rc, ModelRenderer* renderer);

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
