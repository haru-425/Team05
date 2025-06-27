#include"CameraController.h"
#include"Camera.h"
#include"System/Input.h"
#include "imgui.h"

//更新処理
void CameraController::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisRX();
    float ay = gamePad.GetAxisRY();
    //カメラ回転速度
    float speed = rollSpeed*elapsedTime;

    angle.x += ay*elapsedTime;// * speed;
    angle.y -= ax* speed;

    //X軸のカメラ回転を制限
    if (angle.x < minAngleX)
    {
        angle.x = minAngleX;
    }
    if (angle.x > maxAngleX)
    {
        angle.x = maxAngleX;
    }
    //Y軸の回転値を-3.14~3.14に収まるように
    if (angle.y < -DirectX::XM_PI)
    {
        angle.y += DirectX::XM_2PI;
    }
    if (angle.x > DirectX::XM_PI)
    {
        angle.y-=DirectX::XM_2PI;
    }
    //カメラ回転値を回転行列に変換
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    DirectX::XMVECTOR Front = Transform.r[2];//Frontカメラマンの前方　回転後のカメラの情報が計算できる
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Front);

    DebugGUI();

    DirectX::XMFLOAT3 eye;
    target.y += 0.4f;
    eye.x = target.x - front.x * range;
    eye.y = target.y - front.y * range;
    eye.z = target.z - front.z * range;

    //カメラの視点と注視点を設定
    Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
}

void CameraController::DebugGUI()
{
    ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
    ImGui::SetNextWindowPos(ImVec2(pos.x + 300, pos.y + 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_None))
    {
        if (ImGui::Button("Reset"))
        {
            range = 20;
        }
        ImGui::DragFloat("range", &range,0.01f,0.1);
    }
    ImGui::End();
}
