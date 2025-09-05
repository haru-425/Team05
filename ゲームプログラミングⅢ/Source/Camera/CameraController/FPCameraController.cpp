#include "FPCameraController.h"
#include "System/Input.h"
#include <algorithm>
#include <Xinput.h>
#include <DirectXMath.h>
#include "Camera/Camera.h"
#include "imgui.h"
#include "System/SettingsManager.h"

void FPCameraController::Update(float dt)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    //////////////////////////////////////////////////////////////
    //一人称視点
    //////////////////////////////////////////////////////////////

    static constexpr float MAX_PITCH = 89.9f * 0.01745f;
    float sensitivity = 0.005f * SettingsManager::Instance().GetGameSettings().sensitivity; // マウス感度

    float lStick = 0, rStick = 0;                           ///< コントローラー
    float mouseX = 0, mouseY = 0, screenW = 0, screenH = 0; ///< マウス用

    bool useController = gamePad.GetUseController();

    /// コントローラー使用中
    if (useController)
    {
        lStick = gamePad.GetAxisRX();
        rStick = gamePad.GetAxisRY();
    }
    /// マウス
    else
    {
        mouseX = mouse.GetPositionX();
        mouseY = mouse.GetPositionY();
        screenW = mouse.GetScreenWidth();
        screenH = mouse.GetScreenHeight();
    }
    
    // グローバルまたはメンバ変数として保持
    static float targetYaw = 0.0f;
    static constexpr float ROTATE_SPEED = 10.0f; // 回転速度（ラジアン/秒）

    if (!useEnemyCam && !isEvent)
    {
        if (!isRotating) {
            /// マウス
            angle.y += (mouseX - screenW / 2) * sensitivity;
            angle.x += (mouseY - screenH / 2) * sensitivity;

            /// コントローラー
            int correctionValue = 10; ///< 補正値
            angle.y += lStick * sensitivity * correctionValue;
            angle.x -= rStick * sensitivity * correctionValue * 0.7;
        }

        static bool spacePressedLastFrame = false;
        bool spacePressedNow = (gamePad.GetButtonDown() & GamePad::BTN_LEFT_TRIGGER) != 0;

        if (spacePressedNow && !spacePressedLastFrame && !isRotating)
        {
            targetYaw = angle.y + DirectX::XM_PI;

            // Wrap targetYaw to [-π, π] 範囲に収める（必要なら）
            if (targetYaw > DirectX::XM_PI)
                targetYaw -= DirectX::XM_2PI;

            isRotating = true;
        }
        spacePressedLastFrame = spacePressedNow;
        if (isRotating)
        {
            // 線形補間で徐々に回転
            DirectX::XMVECTOR currentYaw = DirectX::XMVectorSet(angle.y, 0, 0, 0);
            DirectX::XMVECTOR targetYawVec = DirectX::XMVectorSet(targetYaw, 0, 0, 0);
            DirectX::XMVECTOR result = DirectX::XMVectorLerp(currentYaw, targetYawVec, ROTATE_SPEED * dt);
            angle.y = DirectX::XMVectorGetX(result);

            // 目標角度に十分近づいたら補間終了
            if (fabsf(angle.y - targetYaw) < 0.001f)
            {
                angle.y = targetYaw;
                isRotating = false;
            }
        }

    }
    // 90度だとバグるので
    angle.x = std::clamp(angle.x, -MAX_PITCH, MAX_PITCH);

    // カメラ切り替え時に保存しておいたptich, yawを使う
    if (isChange)
    {
        angle.x = savePitchYaw.x;
        angle.y = savePitchYaw.y;
    }

    // プレイヤーがカメラを使っている場合随時更新
    // 切り替え時に保存の方がいいかも
    if (!useEnemyCam)
    {
        savePitchYaw.x = pitch;
        savePitchYaw.y = yaw;
    }
    pitch = angle.x;
    yaw = angle.y;

    //DirectX::XMVECTOR forward = DirectX::XMVectorSet(
    //    cosf(pitch) * sinf(yaw),
    //    sinf(pitch),
    //    cosf(pitch) * cosf(yaw),
    //    0.0f
    //);

    DirectX::XMVECTOR Forward = DirectX::XMVectorZero(), Up = DirectX::XMVectorZero();
    DirectX::XMMATRIX RotMat = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    Forward = RotMat.r[2];
    Up = RotMat.r[1];

    // 右方向（横移動用）
    //DirectX::XMVECTOR right = DirectX::XMVector3Cross(forward, DirectX::XMVectorSet(0, 1, 0, 0));

    /* float speed = 10.0f * elapsedTime;

         if ((GetAsyncKeyState('W') & 0x8000) != 0) cameraPos += forward * speed;
         if ((GetAsyncKeyState('S') & 0x8000) != 0) cameraPos -= forward * speed;
         if ((GetAsyncKeyState('A') & 0x8000) != 0) cameraPos += right * speed;
         if ((GetAsyncKeyState('D') & 0x8000) != 0) cameraPos -= right * speed;
    */

    DirectX::XMVECTOR eye = DirectX::XMLoadFloat3(&cameraPos);
    DirectX::XMVECTOR targetVec = DirectX::XMVectorAdd(eye, Forward);
    DirectX::XMFLOAT3 up;
    DirectX::XMStoreFloat3(&up, Up);

    DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(eye, targetVec, Up);
    DirectX::XMStoreFloat3(&target, targetVec);
    DirectX::XMStoreFloat3(&cameraPos, eye);
    Camera::Instance().SetLookAt(cameraPos, target, up);
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
