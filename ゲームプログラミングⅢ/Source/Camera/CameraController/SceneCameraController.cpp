#include "SceneCameraController.h"
#include "Camera/Camera.h"
#include "imgui.h"
#include <stdio.h>

SceneCameraController::SceneCameraController()
{
    SettingViewPointInfo();

    timer = cameraChangeLimit;
}

SceneCameraController::~SceneCameraController()
{
}

void SceneCameraController::Update(float dt)
{
    DirectX::XMFLOAT3 angle = viewPointInfo->angle;
    DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR Front = Rotation.r[2];
    DirectX::XMVECTOR Right = Rotation.r[0];
    DirectX::XMFLOAT3 front, right;
    DirectX::XMStoreFloat3(&front, Front);
    DirectX::XMStoreFloat3(&right, Right);

    DirectX::XMFLOAT3 target = viewPointInfo[viewPoint].target;

    DirectX::XMFLOAT3 eye;
    eye.x = target.x - front.x * viewPointInfo[viewPoint].distance;
    eye.y = target.y - front.y * viewPointInfo[viewPoint].distance;
    eye.z = target.z - front.z * viewPointInfo[viewPoint].distance;
    Camera::Instance().SetLookAt(eye, viewPointInfo[viewPoint].target, DirectX::XMFLOAT3(0, 1, 0));

    /// カメラが切り替わるまでの時間更新
    //timer -= dt;

    /// 時間が０未満になるとカメラ切り替え
    if (timer < 0)
    {
        viewPoint++;
        timer = cameraChangeLimit;

        if (viewPoint >= 5)
            viewPoint = 0;
    }
}

void SceneCameraController::DebugGUI()
{
    if (ImGui::Begin("SceneCameraInfo"))
    {
        ImGui::DragFloat3("target", &viewPointInfo[viewPoint].target.x);
        ImGui::DragFloat3("viewPosition", &viewPointInfo[viewPoint].viewPosition.x);
        ImGui::DragFloat3("angle", &viewPointInfo[viewPoint].angle.x, 0.001745/10, -1, 1);
        ImGui::DragFloat("distance", &viewPointInfo[viewPoint].distance, 0.01f, 0.0001f);

        /// カメラの設置した場所
        ImGui::InputInt("viewPoint", &viewPoint);
        if (viewPoint >= VIEW_POINT_COUNT)
            viewPoint = 4; ///< 最大要素数を超えないように

        ImGui::Separator();
        ImGui::BulletText("CameraChangeLimit");
        char buffer[256];
        sprintf_s(buffer, "timer :%f", timer);
        ImGui::Text(buffer);
    }
    ImGui::End();
}

/**
* @brief カメラの初期設定をする関数
* 
* カメラの位置、焦点、距離などを設定する
*/
void SceneCameraController::SettingViewPointInfo()
{
    {
        viewPointInfo[0].angle = {0.185, -0.111, 0};
        viewPointInfo[0].distance = 0.1;
        viewPointInfo[0].target = {1.8,2.8,-21};
        viewPointInfo[0].viewPosition;
    }
    {
        viewPointInfo[1].angle;
        viewPointInfo[1].distance = 30;
        viewPointInfo[1].target = {0, 0.2f, 3};
        viewPointInfo[1].viewPosition;
    }
    {
        viewPointInfo[2].angle;
        viewPointInfo[2].distance;
        viewPointInfo[2].target;
        viewPointInfo[2].viewPosition;
    }
    {
        viewPointInfo[3].angle;
        viewPointInfo[3].distance;
        viewPointInfo[3].target;
        viewPointInfo[3].viewPosition;
    }
    {
        viewPointInfo[4].angle;
        viewPointInfo[4].distance;
        viewPointInfo[4].target;
        viewPointInfo[4].viewPosition;
    }
}
