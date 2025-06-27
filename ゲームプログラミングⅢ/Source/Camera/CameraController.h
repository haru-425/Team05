#pragma once

#include<DirectXMath.h>

//�J�����R���g���[���[
class CameraController
{
public:
    //�X�V����
    void Update(float elapsedTime);

    void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

    void DebugGUI();

    float GetFoV() const { return fov; }

private:
    DirectX::XMFLOAT3  target = { 0,0,0 };
    DirectX::XMFLOAT3  angle = { 45,0,0 };
    float          rollSpeed = DirectX::XMConvertToRadians(90);
    float          range = 20;//�J�����̋���
    float          maxAngleX = DirectX::XMConvertToRadians(45);//�J�����̊p�x
    float          minAngleX = DirectX::XMConvertToRadians(-45);//�T

    float fov = 0;
};

