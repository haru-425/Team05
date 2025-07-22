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
    DirectX::XMFLOAT3 angle = viewPointInfo[viewPoint].angle;
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
    timer -= dt;

    bool isAngleMax = false;

    /// 時間が０未満になるとカメラ切り替え
    if (timer < 0)
    {
        viewPoint++;
        timer = cameraChangeLimit;

        if (viewPoint >= 5)
            viewPoint = 0;
    }

    /// カメラ4番目を首振りにする
    viewPointInfo[3].angle.y += angler * dt;

    if (viewPointInfo[3].angle.y > 3.89)
    {
        viewPointInfo[3].angle.y = 3.89;
        isAngleMax = true;
    }
    else if (viewPointInfo[3].angle.y < 2.40)
    {
        viewPointInfo[3].angle.y = 2.40;
        isAngleMax = true;
    }

    if (isAngleMax)
    {
        coolTime -= dt;
        if (coolTime < 0)
        {
            coolTime = 3;
            isChangeDir = true;
        }
    }

    if (isChangeDir)
    {
        angler = -angler;
        isChangeDir = false;
    }
}

void SceneCameraController::DebugGUI()
{
    if (ImGui::Begin("SceneCameraInfo"))
    {
        ImGui::DragFloat3("target", &viewPointInfo[viewPoint].target.x);
        ImGui::DragFloat3("angle", &viewPointInfo[viewPoint].angle.x, 0.001745/10);
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
        viewPointInfo[0].angle = {0.185, 0, 0};
        viewPointInfo[0].distance = 0.1;
        viewPointInfo[0].target = {1.0,2.4,-25};
    }
    {
        viewPointInfo[1].angle = {0.264, -0.945, 0};
        viewPointInfo[1].distance = 0.1f;
        viewPointInfo[1].target = {-6.2, 2.8f, -15.9};
    }
    {
        viewPointInfo[2].angle = {0.196,4.71,0};
        viewPointInfo[2].distance = 0.1f;
        viewPointInfo[2].target = {28.8,2.1,-23};
    }
    {
        viewPointInfo[3].angle = {0.367, 3.141, 0};
        viewPointInfo[3].distance = 0.1f;
        viewPointInfo[3].target = {0, 2.8f, 24.9};
    }
    {
        viewPointInfo[4].angle = {0.300, 3.14, 0};
        viewPointInfo[4].distance = 0.1f;
        viewPointInfo[4].target = {0, 2.6, 13.9};
    }
}
