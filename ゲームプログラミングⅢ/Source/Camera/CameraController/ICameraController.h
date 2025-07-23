#pragma once
#include <DirectXMath.h>

class ICameraController
{
public:
    //�X�V����
    virtual void Update(float dt) = 0;

    void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }
    void SetCameraPos(const DirectX::XMFLOAT3& cameraPos) { this->cameraPos = cameraPos; }

    virtual void DebugGUI() {}

    float GetFoV() const { return fov; }

    void SetIsChange(bool isChange) { this->isChange = isChange; }
    void SetUseEnemyCam(bool useEnemyCam) { this->useEnemyCam = useEnemyCam; }

    float GetPitch() const { return pitch; }
    float GetYaw() const { return yaw; }

    void SetPitch(float pitch) { this->pitch = pitch; }
    void SetYaw(float yaw) { this->yaw = yaw; }

    void SetIsEvent(bool isEvent) { this->isEvent = isEvent; }

protected:
    DirectX::XMFLOAT3   target = { 0,0,0 };                 // �O�l�̃J�����Ɏg��
    DirectX::XMFLOAT3   cameraPos = { 0.0f, 2.0f, -5.0f };  // ��l�̃J�����Ɏg���A�J�����̈ʒu�₯�ǃv���C���[�Ɏ������邩��Ӗ��͂Ȃ�
    DirectX::XMFLOAT3   angle = { 45,0,0 };
    float               rollSpeed = DirectX::XMConvertToRadians(90);
    float               range = 10;//�J�����̋���
    float               maxAngleX = DirectX::XMConvertToRadians(45);//�J�����̊p�x
    float               minAngleX = DirectX::XMConvertToRadians(-45);//�T
    // ��l��
    float yaw = 0.0f;   // ���E��]
    float pitch = 0.0f; // �㉺��]

    float fov = 0;

    DirectX::XMFLOAT3 savePosition; // �t���[�J�����p
    DirectX::XMFLOAT2 savePitchYaw; // FPC�p

    bool isChange = false;
    bool useEnemyCam = false;
    bool isEvent = false;
};

