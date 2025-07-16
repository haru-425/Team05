#pragma once
#include"System/ModelRenderer.h"
#include"System/ShapeRenderer.h"
#include "System/LoadTextures.h"

#include <memory>
#include <wrl.h>

class Object
{
public:
    Object() {}
    Object(DirectX::XMFLOAT3 position,Model* model,LoadTextures* textures);
    ~Object() {}

    // �R�s�[�֎~
    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;

    // ���[�u����
    Object(Object&&) noexcept = default;
    Object& operator=(Object&&) noexcept = default;

    //�X�e�[�W
    void Update(float elapsedTime);

    //�`�揈��
    void Render(const RenderContext& rc, ModelRenderer* renderer);
    void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

    void DebugGUI();

    DirectX::XMFLOAT3 GetPosition() { return position; }

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

    std::unique_ptr<Model> model = nullptr;

    std::unique_ptr<LoadTextures> textures;
};

