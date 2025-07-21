#include "LightDebugCameraController.h"
#include "System/Input.h"
#include "Camera/Camera.h"
#include <imgui.h>
#include <algorithm>

void LightDebugCameraController::Update(float dt)
{
    GamePad& gamepad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();
    target.y = 1.5f;

    // IMGUI�̃}�E�X���͒l���g���ăJ�������삷��
    ImGuiIO io = ImGui::GetIO();

    if (Mouse::BTN_RIGHT & mouse.GetButton()) {

        // �}�E�X�J�[�\���̈ړ��ʂ����߂�
        float moveX = io.MouseDelta.x;
        float moveY = io.MouseDelta.y;
        float sensitivity = 0.005f; // ��]���x


        angle.x += io.MouseDelta.y * sensitivity;
        angle.y += io.MouseDelta.x * sensitivity;

        angle.x = std::clamp(angle.x, minAngleX, DirectX::XMConvertToRadians(85));

        // Y���̉�]�� -�� ~ �� �Ɏ��߂�
        if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
        if (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;
    }

    // WASD�L�[�ɂ��ړ�

    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR Front = Transform.r[2];
    DirectX::XMVECTOR Right = Transform.r[0];

    float moveSpeed = 8.0f * dt;

        if (GetAsyncKeyState('W') & 0x8000) {
            target.x += DirectX::XMVectorGetX(Front) * moveSpeed;
            target.z += DirectX::XMVectorGetZ(Front) * moveSpeed;
        }
        if (GetAsyncKeyState('S') & 0x8000) {
            target.x -= DirectX::XMVectorGetX(Front) * moveSpeed;
            target.z -= DirectX::XMVectorGetZ(Front) * moveSpeed;
        }
        if (GetAsyncKeyState('A') & 0x8000) {
            target.x -= DirectX::XMVectorGetX(Right) * moveSpeed;
            target.z -= DirectX::XMVectorGetZ(Right) * moveSpeed;
        }
        if (GetAsyncKeyState('D') & 0x8000) {
            target.x += DirectX::XMVectorGetX(Right) * moveSpeed;
            target.z += DirectX::XMVectorGetZ(Right) * moveSpeed;
        }


    int wheelDelta = io.MouseWheel; // �z�C�[���̃X�N���[���ʂ��擾
    float zoomSpeed = 45.0f; // �Y�[�����x

    range -= wheelDelta * zoomSpeed * dt;

    // �Y�[���͈͂̐����i�߂����E�������h�~�j
    range = std::clamp(range, 1.0f, 100.0f);


    // �J�����̎��_�ݒ�
    DirectX::XMFLOAT3 eye;
    eye.x = target.x - DirectX::XMVectorGetX(Front) * range;
    eye.y = target.y - DirectX::XMVectorGetY(Front) * range;
    eye.z = target.z - DirectX::XMVectorGetZ(Front) * range;

    Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));

    DebugGUI();
}

void LightDebugCameraController::DebugGUI()
{
}
