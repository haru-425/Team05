#include"CameraController.h"
#include"Camera.h"
#include"System/Input.h"
#include "imgui.h"

//�X�V����
void CameraController::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisRX();
    float ay = gamePad.GetAxisRY();
    //�J������]���x
    float speed = rollSpeed*elapsedTime;

    angle.x += ay*elapsedTime;// * speed;
    angle.y -= ax* speed;

    //X���̃J������]�𐧌�
    if (angle.x < minAngleX)
    {
        angle.x = minAngleX;
    }
    if (angle.x > maxAngleX)
    {
        angle.x = maxAngleX;
    }
    //Y���̉�]�l��-3.14~3.14�Ɏ��܂�悤��
    if (angle.y < -DirectX::XM_PI)
    {
        angle.y += DirectX::XM_2PI;
    }
    if (angle.x > DirectX::XM_PI)
    {
        angle.y-=DirectX::XM_2PI;
    }
    //�J������]�l����]�s��ɕϊ�
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    DirectX::XMVECTOR Front = Transform.r[2];//Front�J�����}���̑O���@��]��̃J�����̏�񂪌v�Z�ł���
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Front);

    DebugGUI();

    DirectX::XMFLOAT3 eye;
    target.y += 0.4f;
    eye.x = target.x - front.x * range;
    eye.y = target.y - front.y * range;
    eye.z = target.z - front.z * range;

    //�J�����̎��_�ƒ����_��ݒ�
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
