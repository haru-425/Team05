#include "Player.h"
#include "System/Input.h"
#include "Camera/Camera.h"
#include "imgui.h"

Player::Player()
{
#ifdef TEST
    // �m�F�p
    model = std::make_unique<Model>("./Data/Model/Test/gun.mdl");
    t_position.x += 0.2f;
    t_position.z += 0.5f;
    t_position.y = 1.15f;
    t_scale = { 0.025,0.025,0.025 };

#else
    // ���ۂɎg�����f��
    model = std::make_unique<Model>("./Data/Model/")
#endif

    // �v���C���[�̃p�����[�^�����ݒ�
    viewPoint = 1.5;   // �J�����̖ڐ���ݒ肷�邽��
    radius = 2;        // �f�o�b�O�p
    enableHijackTime = maxHijackTime;
}

Player::~Player()
{
}

void Player::Update(float dt)
{
    // �n�C�W���b�N�̎��ԏ���
    UpdateHijack(dt);

    // �J�����؂�ւ�����
    ChangeCamera();

    // �ړ�����
    Move(dt);

#ifdef TEST
    TestTransformUpdate();
#endif
    // �V�[���̍Ō�ɂ����邱�Ƃɂ��܂�
    // �v���C���[�̍s���A�s��X�V
    // ��
    // �R���W����
    // ��
    // �s��X�V
    // 
    // �s��X�V����
    UpdateTransform();
}

// �`�揈��
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
#ifndef TEST
    if(model)
        renderer->Render(rc, world, model.get(), ShaderId::Lambert);
#else
    DirectX::XMMATRIX T_T = DirectX::XMLoadFloat4x4(&t_transform);
    DirectX::XMMATRIX PT = DirectX::XMLoadFloat4x4(&world);
    T_T = DirectX::XMMatrixMultiply(T_T, PT);
    DirectX::XMStoreFloat4x4(&t_transform, T_T);

    if (model)
        renderer->Render(rc, t_transform, model.get(), ShaderId::Lambert);
#endif
}

// �f�o�b�O�`�揈��
void Player::DrawDebug()
{
    if (ImGui::Begin("Player", nullptr))
    {
        ImGui::InputFloat3("saveDirection", &saveDirection.x);
        ImGui::InputFloat3("position", &position.x);

        DirectX::XMFLOAT3 playerAngle = { angle.x / 0.01745f, angle.y / 0.01745f, angle.z / 0.01745f };
        ImGui::InputFloat3("angle", &playerAngle.x);
        DirectX::XMFLOAT3 camDir = Camera::Instance().GetFront();
        ImGui::InputFloat3("camDir", &camDir.x);

        char text[256];
        sprintf_s(text, "HijackTimer %f", enableHijackTime);
        ImGui::Text(text);
    }
    ImGui::End();
}

// �ړ�����
void Player::Move(float dt)
{
    Camera& cam = Camera::Instance();

    DirectX::XMFLOAT3 forward;
    // �J�������؂�ւ���Ă��Ȃ��Ƃ������J�����̕��������
    if (!useCam)
    {
        forward = cam.GetFront();
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
    saveDirection = forward;

    position.x += speed * forward.x * dt;
    position.z += speed * forward.z * dt;

    // �p�x�����߂�
    {
        DirectX::XMFLOAT3 front = { 0,0,1 };
        DirectX::XMVECTOR Front, PlayerDir;
        Front = DirectX::XMLoadFloat3(&front);
        PlayerDir = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&forward));

        DirectX::XMVECTOR Dot, Cross;
        DirectX::XMFLOAT3 crossVector;
        float dot;
        Dot = DirectX::XMVector3Dot(Front, PlayerDir);
        Cross = DirectX::XMVector3Cross(Front, PlayerDir);
        DirectX::XMStoreFloat(&dot, Dot);
        DirectX::XMStoreFloat3(&crossVector, Cross);

        float radian = acosf(dot);

        if (crossVector.y < 0)
            radian *= -1;

        angle.y = radian;
    }
}

// �J�����؂�ւ������A���ۂ̃J�����؂�ւ��͊O���ł���
void Player::ChangeCamera()
{
    Mouse& mouse = Input::Instance().GetMouse();

    if (isChange)isChange = false; // ��񂾂��ʂ�����
    if (isHijack)isHijack = false;

    // �J�����؂�ւ�
    if (mouse.GetButtonDown() & Mouse::BTN_LEFT && enableHijack)
    {
        if (useCam)
            isChange = true;
        else
            isHijack = true;

        useCam = !useCam;
    }

    // �Q�[�W���Ȃ��Ȃ�Ƌ����I�ɖ߂�
    if (enableHijackTime <= 0 && useCam)
    {
        useCam = false;
        isChange = true;
    }
}

// �n�C�W���b�N�Q�[�W�̍X�V����
void Player::UpdateHijack(float dt)
{

    enableHijack = true;
    if (enableHijackTime < 8.0f && !useCam)
        enableHijack = false;

    // ���E�ύX�ɖ��萔�̃Q�[�W�̌���
    if (isHijack)
    {
        // ��萔�̃Q�[�W����
        enableHijackTime -= hijackCost;
    }

    // �J�������n�C�W���b�N���Ă���ꍇ
    if (useCam)
    {
        // �Q�[�W�̏���
        enableHijackTime -= hijackCostPerSec * dt;
    }
    // ���E���v���C���[�̏ꍇ
    else
    {
        // �n�C�W���b�N�ł��鎞�Ԃ��n�C�W���b�N�ł���ő厞�Ԃ�菬�����ꍇ
        // �Q�[�W�̉�
        // �Q�[�W�̐���
        if (maxHijackTime > enableHijackTime)
        {
            enableHijackTime += hijackRecoveryPerSec * dt;

            if (enableHijackTime > maxHijackTime)
                enableHijackTime = maxHijackTime;
        }
    }
}