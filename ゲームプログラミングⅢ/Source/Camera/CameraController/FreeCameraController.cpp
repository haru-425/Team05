#include "FreeCameraController.h"
#include "System/Input.h"
#include "Camera/Camera.h"

void FreeCameraController::Update(float dt)
{
    GamePad& gamepad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    float lx = gamepad.GetAxisLX();
    float ly = gamepad.GetAxisLY();
    float rx = gamepad.GetAxisRX();
    float ry = gamepad.GetAxisRY();

    //�J������]���x
    float speed = rollSpeed * dt;

    angle.x += ry * speed;// * speed;
    angle.y -= rx * speed;

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
        angle.y -= DirectX::XM_2PI;
    }


    //�J������]�l����]�s��ɕϊ�
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    DirectX::XMVECTOR Front = Transform.r[2];//Front�J�����}���̑O���@��]��̃J�����̏�񂪌v�Z�ł���
    DirectX::XMVECTOR Right = Transform.r[0];
    DirectX::XMFLOAT3 front, right;
    DirectX::XMStoreFloat3(&front, Front);
    DirectX::XMStoreFloat3(&right, Right);

    float f = sqrtf(front.x * front.x + front.y * front.y + front.z * front.z);
    float r = sqrtf(right.x * right.x + right.y * right.y + right.z * right.z);
    //target.x += lx * r * 10 * dt;
    //target.y += lx * f * 10 * dt;
    //target.z += ly * f * 10 * dt;

    target.x += right.x * lx * 10 * dt;
    target.z += right.z * lx * 10 * dt;

    target.x += front.x * ly * 10 * dt;
    target.z += front.z * ly * 10 * dt;

    DirectX::XMFLOAT3 eye;
    eye.x = target.x - front.x * range;
    eye.y = target.y - front.y * range;
    eye.z = target.z - front.z * range;

    //�J�����̎��_�ƒ����_��ݒ�
    Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));

    DebugGUI();
}

void FreeCameraController::DebugGUI()
{
}
