#include "FPCameraController.h"
#include "System/Input.h"
#include <algorithm>
#include <DirectXMath.h>
#include "Camera/Camera.h"
#include "imgui.h"

void FPCameraController::Update(float dt)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    //////////////////////////////////////////////////////////////
    //��l�̎��_
    //////////////////////////////////////////////////////////////

    static constexpr float MAX_PITCH = 89.9f * 0.01745f;
    float sensitivity = 0.005f; // �}�E�X���x

    if (!useEnemyCam)
    {
        yaw += (mouse.GetPositionX() - mouse.GetScreenWidth() / 2) * sensitivity;
        pitch += -(mouse.GetPositionY() - mouse.GetScreenHeight() / 2) * sensitivity;
    }
    // 90�x���ƃo�O��̂�
    pitch = std::clamp(pitch, -MAX_PITCH, MAX_PITCH);

    // �J�����؂�ւ����ɕۑ����Ă�����ptich, yaw���g��
    if (isChange)
    {
        pitch = savePitchYaw.x;
        yaw = savePitchYaw.y;
    }

    // �v���C���[���J�������g���Ă���ꍇ�����X�V
    // �؂�ւ����ɕۑ��̕�����������
    if (!useEnemyCam)
    {
        savePitchYaw.x = pitch;
        savePitchYaw.y = yaw;
    }


    DirectX::XMVECTOR forward = DirectX::XMVectorSet(
        cosf(pitch) * sinf(yaw),
        sinf(pitch),
        cosf(pitch) * cosf(yaw),
        0.0f
    );

    // �E�����i���ړ��p�j
    DirectX::XMVECTOR right = DirectX::XMVector3Cross(forward, DirectX::XMVectorSet(0, 1, 0, 0));

    /* float speed = 10.0f * elapsedTime;

         if ((GetAsyncKeyState('W') & 0x8000) != 0) cameraPos += forward * speed;
         if ((GetAsyncKeyState('S') & 0x8000) != 0) cameraPos -= forward * speed;
         if ((GetAsyncKeyState('A') & 0x8000) != 0) cameraPos += right * speed;
         if ((GetAsyncKeyState('D') & 0x8000) != 0) cameraPos -= right * speed;
    */

    DirectX::XMVECTOR eye = DirectX::XMLoadFloat3(&cameraPos);
    DirectX::XMVECTOR targetVec = DirectX::XMVectorAdd(eye, forward);
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0, 1, 0, 0);

    DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(eye, targetVec, up);
    DirectX::XMStoreFloat3(&target, targetVec);
    DirectX::XMStoreFloat3(&cameraPos, eye);
    Camera::Instance().SetLookAt(cameraPos, target, DirectX::XMFLOAT3(0, 1, 0));
}

void FPCameraController::DebugGUI()
{
    ImGui::SetNextWindowPos(ImVec2{ 500, 100 });
    ImGui::SetNextWindowSize(ImVec2{ 200, 150 });
    if (ImGui::Begin("FPC"))
    {
        ImGui::InputFloat2("SavePitchYaw", &savePitchYaw.x);
        ImGui::InputFloat("Pitch", &pitch);
        ImGui::InputFloat("Yaw", &yaw);
    }
    ImGui::End();
}
