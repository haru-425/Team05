#pragma once

#include<DirectXMath.h>

//カメラコントローラー
class CameraController
{
public:
    //更新処理
    void Update(float elapsedTime);

    void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

    void DebugGUI();

    float GetFoV() const { return fov; }

private:
    DirectX::XMFLOAT3  target = { 0,0,0 };
    DirectX::XMFLOAT3  angle = { 45,0,0 };
    float          rollSpeed = DirectX::XMConvertToRadians(90);
    float          range = 20;//カメラの距離
    float          maxAngleX = DirectX::XMConvertToRadians(45);//カメラの角度
    float          minAngleX = DirectX::XMConvertToRadians(-45);//亀

    float fov = 0;
};

