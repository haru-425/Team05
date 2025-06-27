#pragma once

#include<DirectXMath.h>
#include"Camera/CameraController.h"
#include"System/ShapeRenderer.h"

//�L�����N�^�[
class Character
{
public:
    Character() {}
    virtual ~Character(){}

    //�s��X�V����
    virtual void UpdateTransform();

    //�ʒu�擾
    const DirectX::XMFLOAT3& GetPosition() const { return position; }

    //�ʒu�ݒ�
    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

    //��]�擾
    void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

    //�X�P�[���擾
    const DirectX::XMFLOAT3& GetScale() const { return scale; }

    //�X�P�[���擾
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

    //���a�擾
    float GetRadius() const { return radius; }

    //�f�o�b�O�v���~�e�B�u�`��
    virtual void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

    //�n�ʂɐڒn���Ă��邩
    bool IsGround()const { return isGround; }

    float GetHeight()const { return height; }

    //�p�[�e�B�N������
    //virtual void RenderBox

    const DirectX::XMFLOAT4X4 GetTransform() const { return transform; }

    bool ApplyDamage(int damage, float invincibleTime);

    void AddImpulse(const DirectX::XMFLOAT3& impulse);
protected:
    //�ړ����͏���
    void InputMove(float elapsedTime);

    //���񏈗�
    void Turn(float elapsedTime, float vx, float vz, float speed);

    //�W�����v����
    void Jump(float speed);

    void Move(float elapsedTime, float vx, float vz, float speed);

    //���͏����X�V
    void UpdateVelocity(float elapsedTime);

    void UpdateInvicibleTimer(float elapsedTime);

    //���n�������ɌĂ΂��
    virtual void OnLanding() {}

    virtual void OnDamage() {}
    virtual void OnDead() {}

private:
    void UpdateVerticalVelocity(float elapsedTime);
    void UpdateVerticalMove(float elapsedTime);
    void UpdateHorizontalVelocity(float elapsedTime);
    void UpdateHorizontalMove(float elapsedTime);

protected:
    DirectX::XMFLOAT3     position = { 0,0,0 };
    DirectX::XMFLOAT3     angle = { 0,0,0 };
    DirectX::XMFLOAT3     scale = { 1,1,1 };
    DirectX::XMFLOAT4X4   transform = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };

    float radius = 0.5f;
    float height = 2.0f;
    bool isGround = false;

    int health = 5;
    float invincibleTimer = 1.0f;
    float friction = 15.0f;
    float acceleration = 50.0f;
    float maxMoveSpeed = 5.0f;
    float moveVecX = 0.0f;
    float moveVecZ = 0.0f;
    float airControl = 0.3f;
private:
    //CameraController* cameraController = nullptr;
    float gravity = -30.0f;
    DirectX::XMFLOAT3 velocity = { 0,0,0 };
};