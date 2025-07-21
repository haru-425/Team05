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

    // IMGUIのマウス入力値を使ってカメラ操作する
    ImGuiIO io = ImGui::GetIO();

    if (Mouse::BTN_RIGHT & mouse.GetButton()) {

        // マウスカーソルの移動量を求める
        float moveX = io.MouseDelta.x;
        float moveY = io.MouseDelta.y;
        float sensitivity = 0.005f; // 回転感度


        angle.x += io.MouseDelta.y * sensitivity;
        angle.y += io.MouseDelta.x * sensitivity;

        angle.x = std::clamp(angle.x, minAngleX, DirectX::XMConvertToRadians(85));

        // Y軸の回転を -π ~ π に収める
        if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
        if (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;
    }

    // WASDキーによる移動

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


    int wheelDelta = io.MouseWheel; // ホイールのスクロール量を取得
    float zoomSpeed = 45.0f; // ズーム速度

    range -= wheelDelta * zoomSpeed * dt;

    // ズーム範囲の制限（近すぎ・遠すぎ防止）
    range = std::clamp(range, 1.0f, 100.0f);


    // カメラの視点設定
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
