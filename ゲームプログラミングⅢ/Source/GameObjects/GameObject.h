#pragma once
#include "System/RenderContext.h"
#include "System/ModelRenderer.h"
#include "System/ShapeRenderer.h"

// �f�o�b�O�`�惂�[�h�̑I��
static enum DEBUG_MODE
{
    BOX         = 1 << 0,
    CAPSULE     = 1 << 1,
    CYLINDER    = 1 << 2,
    SPHERE      = 1 << 3,
};

inline DEBUG_MODE operator|(DEBUG_MODE a, DEBUG_MODE b)
{
    return static_cast<DEBUG_MODE>(static_cast<int>(a) | static_cast<int>(b));
}

class GameObject
{
public:
    GameObject() = default;
    virtual ~GameObject() = default;

    // �X�V����
    virtual void Update(float dt) = 0;

    // �`�揈��
    virtual void Render(const RenderContext& rc, ModelRenderer* renderer) = 0;

    // �f�o�b�O�`��
    void RenderDebug(const RenderContext& rc, ShapeRenderer* renderer, DirectX::XMFLOAT3 size, DirectX::XMFLOAT4 color, DEBUG_MODE debugMode)
    {
        if (debugMode & DEBUG_MODE::BOX)
            renderer->RenderBox(rc, position, angle, size, color);
        if (debugMode & DEBUG_MODE::CAPSULE)
            renderer->RenderCapsule(rc, world, radius, size.y, color);
        if (debugMode & DEBUG_MODE::CYLINDER)
            renderer->RenderCylinder(rc, position, radius, size.y, color);
        if (debugMode & DEBUG_MODE::SPHERE)
            renderer->RenderSphere(rc, position, radius, color);
    }

    // ImGui�`�揈��
    virtual void DrawDebug() {};

    // �s��X�V����
    void UpdateTransform()
    {
        //�X�P�[���s����쐬
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.x);
        //��]�s��
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
        //�ʒu�s����쐬
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
        //�R�̍s���g�ݍ��킹�A���[���h�s����쐬
        DirectX::XMMATRIX W = S * R * T;
        //�v�Z�������[���h�s������o��
        DirectX::XMStoreFloat4x4(&world, W);
    }

    // �Q�b�^�[
    DirectX::XMFLOAT3 GetPosition() const { return position; }
    DirectX::XMFLOAT3 GetAngle() const { return angle; }
    DirectX::XMFLOAT3 GetScale() const { return scale; }
    DirectX::XMFLOAT4X4 GetWorld() const { return world; }

    float GetViewPoint() const { return viewPoint; }

    float GetRadius() const { return radius; }

    // �Z�b�^�[
    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
    void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

    void SetIsHit(bool isHit) { this->isHit = isHit; }

protected:
    DirectX::XMFLOAT3 position = { 0,0,0 };
    DirectX::XMFLOAT3 angle = { 0,0,0 };
    DirectX::XMFLOAT3 scale = { 1,1,1 };
    DirectX::XMFLOAT4X4 world = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };

    float viewPoint = 0;

    DirectX::XMFLOAT2 velocity = {};    // ����͗������Ȃ�����float2�^
    float radius = 0;                   // �T���Ɏg���H
    float acceleration = 0;             // �����x
    DirectX::XMFLOAT3 saveDirection;    // �Ō�̃J�����̕�����ۑ����邽��

    bool isHit = false;
};

