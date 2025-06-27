#pragma once
#include <DirectXMath.h>

class ICameraController
{
public:
    //更新処理
    virtual void Update(float dt) = 0;

    void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }
    void SetCameraPos(const DirectX::XMFLOAT3& cameraPos) { this->cameraPos = cameraPos; }

    virtual void DebugGUI() {}

    float GetFoV() const { return fov; }

protected:
    DirectX::XMFLOAT3   target = { 0,0,0 };                 // 三人称カメラに使う
    DirectX::XMFLOAT3   cameraPos = { 0.0f, 2.0f, -5.0f };  // 一人称カメラに使う、カメラの位置やけどプレイヤーに持たせるから意味はない
    DirectX::XMFLOAT3   angle = { 45,0,0 };
    float               rollSpeed = DirectX::XMConvertToRadians(90);
    float               range = 20;//カメラの距離
    float               maxAngleX = DirectX::XMConvertToRadians(45);//カメラの角度
    float               minAngleX = DirectX::XMConvertToRadians(-45);//亀
    // 一人称
    float yaw = 0.0f;   // 左右回転
    float pitch = 0.0f; // 上下回転

    float fov = 0;
};

