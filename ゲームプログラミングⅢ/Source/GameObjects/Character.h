#pragma once

#include<DirectXMath.h>
#include"Camera/CameraController.h"
#include"System/ShapeRenderer.h"

//キャラクター
class Character
{
public:
    Character() {}
    virtual ~Character(){}

    //行列更新処理
    virtual void UpdateTransform();

    //位置取得
    const DirectX::XMFLOAT3& GetPosition() const { return position; }

    //位置設定
    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

    //回転取得
    void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

    //スケール取得
    const DirectX::XMFLOAT3& GetScale() const { return scale; }

    //スケール取得
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

    //半径取得
    float GetRadius() const { return radius; }

    //デバッグプリミティブ描画
    virtual void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

    //地面に接地しているか
    bool IsGround()const { return isGround; }

    float GetHeight()const { return height; }

    //パーティクル実験
    //virtual void RenderBox

    const DirectX::XMFLOAT4X4 GetTransform() const { return transform; }

    bool ApplyDamage(int damage, float invincibleTime);

    void AddImpulse(const DirectX::XMFLOAT3& impulse);
protected:
    //移動入力処理
    void InputMove(float elapsedTime);

    //旋回処理
    void Turn(float elapsedTime, float vx, float vz, float speed);

    //ジャンプ処理
    void Jump(float speed);

    void Move(float elapsedTime, float vx, float vz, float speed);

    //速力処理更新
    void UpdateVelocity(float elapsedTime);

    void UpdateInvicibleTimer(float elapsedTime);

    //着地した時に呼ばれる
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