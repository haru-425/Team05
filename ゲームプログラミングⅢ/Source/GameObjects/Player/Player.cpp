#include "Player.h"
#include "System/Input.h"
#include "Camera/Camera.h"
#include "imgui.h"

Player::Player()
{
    //model = std::make_unique<Model>("")

    // �v���C���[�̃p�����[�^�����ݒ�
    position.y = 1.5; // �J�����̖ڐ���ݒ肷�邽��
    radius = 2;     // �f�o�b�O�p
}

Player::~Player()
{
}

void Player::Update(float dt)
{
    // �J�����؂�ւ�����
    ChangeCamera();

    // �ړ�����
    Move(dt);

    // �s��X�V����
    UpdateTransform();
}

void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    if(model)
        renderer->Render(rc, world, model.get(), ShaderId::Lambert);
}

void Player::DrawDebug()
{
    if (ImGui::Begin("Player", nullptr))
    {
        ImGui::InputFloat3("saveDirection", &saveDirection.x);
    }
    ImGui::End();
}

void Player::Move(float dt)
{
    Camera& cam = Camera::Instance();

    DirectX::XMFLOAT3 forward;
    // �J�������؂�ւ���Ă��Ȃ��Ƃ������J�����̕��������
    if (isChangeCamera == false)
    {
        forward = cam.GetFront();
        saveDirection = forward;
    }
    else
        forward = saveDirection;


    // �J�����̕�����XZ�ʂɌŒ�
    forward.y = 0;
    float len = sqrtf(forward.x * forward.x + forward.z * forward.z);
    if (len > 0.0f)
    {
        forward.x /= len;
        forward.z /= len;
    }

    position.x += 5 * forward.x * dt;
    position.z += 5 * forward.z * dt;
}

// �J�����؂�ւ������A���ۂ̃J�����؂�ւ��͊O���ł���
void Player::ChangeCamera()
{
    Mouse& mouse = Input::Instance().GetMouse();

    // �J�����؂�ւ�
    if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
    {
        isChangeCamera = !isChangeCamera;

        if (isChangeCamera == false)
            Camera::Instance().SetFront(saveDirection);
    }
}
