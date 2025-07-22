#include "Player.h"
#include "System/Input.h"
#include "Camera/Camera.h"
#include "imgui.h"
#include <algorithm>

static bool hit = false;
Player::Player()
{
#ifdef TEST
    // �m�F�p
    model = std::make_shared<Model>("./Data/Model/Test/test_walk_animation_model.mdl");
    t_position.x += 0.2f;
    t_position.z += 0.5f;
    t_position.y = 1.15f;
    t_scale = { 0.025,0.025,0.025 };

#else
    // ���ۂɎg�����f��
    //model = std::make_unique<Model>("./Data/Model/Player/player.mdl");
    model = std::make_unique<Model>("./Data/Model/Player/player_mesh.mdl");
#endif

    // �v���C���[�̃p�����[�^�����ݒ�
    {
        position = { 1,0,-24 };
        scale = { 0.015, 0.015, 0.015 };    // �X�P�[��
        viewPoint = 1.5;                    // �J�����̖ڐ���ݒ肷�邽��
        radius = 0.6;                         // �f�o�b�O�p
        enableHijackTime = maxHijackTime;   // �n�C�W���b�N�ł��鎞�Ԃ̐ݒ�
        acceleration = 1.1f;
        deceleration = 1.2f;
        hit = false;
    }

    /// �A�j���[�V�����֌W�ݒ�
    {
        animationController.SetModel(model);
        animationController.PlayAnimation(static_cast<int>(AnimationState::MOVE), true);
        animationController.SetAnimationSecondScale(1.0f);
    }

    /// �e�N�X�`���̓ǂݍ���
    textures = std::make_unique<LoadTextures>();
    textures->LoadNormal("Data/Model/Player/Texture/player_mtl_Normal_DirectX.png");
    textures->LoadMetalness("Data/Model/Player/Texture/player_mtl_Metallic.png");
    textures->LoadEmisive("Data/Model/Player/Texture/player_mtl_Emissive.png");
    textures->LoadOcclusion("Data/Model/Player/Texture/player_mtl_Opacity.png");
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

    if (isEvent) ///< Move() �̒��Ńt���O�؂�ւ������Ă�
        DeathState(dt);

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

    // �A�j���[�V�����̍X�V����
    UpdateAnimation(dt);

    // ���f���̍s��X�V����
    model->UpdateTransform();
}

// �`�揈��
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
#ifndef TEST

    /// �e�N�X�`���̃Z�b�g
    textures->Set(rc);

    /// ���f��������Ƃ����A�v���C���[���G�J�������g���Ă���ꍇ
    /// �v���C���[��`�悷��Ƃǂ����Ă��A���f���ƃJ����������Ă��܂��̂ŁA
    /// �G���_�̎��݂̂̕`��ɂ���
    if (model && useCam)
        renderer->Render(rc, world, model.get(), ShaderId::Custom);

    // �e�N�X�`���̃N���A
    textures->Clear(rc);
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

        ImGui::Checkbox("isHit", &isHit);
    }
    ImGui::End();
}

// �ړ�����
void Player::Move(float dt)
{
    if (!hit && isHit) ///< ��肩���͉������ǁA��x�q�b�g�����炻��ȍ~�̓q�b�g����ɂ��邽�߂ɏ��� hit ��Player�R���X�g���N�^�̏�ŃO���[�o���Ƃ��Ă����Ă�
    {
        accel = 0;
        hit = isHit;
    }

    /// �G�ƐڐG�����ꍇ�͂��񂾂񑬓x�𗎂Ƃ��ĉ��o�ɓ���
    if (hit)
    {
        if (speed > 0)
            accel -= deceleration * dt;
        else
            isEvent = true;
    }
    else
    {
        /// ��������
        accel += acceleration * dt;
    }

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

#if 1
    speed += accel * dt;
#else
    if (Input::Instance().GetMouse().GetButton() & Mouse::BTN_RIGHT)
        speed = 3;
    else
        speed = 0;
#endif
    speed = DirectX::XMMin(speed, maxSpeed);
    speed = DirectX::XMMax(speed, 0.0f);
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
    if (isHit)return;

    Mouse& mouse = Input::Instance().GetMouse();

    if (isChange)isChange = false; // ��񂾂��ʂ�����
    if (isHijack)isHijack = false;

    // �J�����؂�ւ�
    if (mouse.GetButtonDown() & Mouse::BTN_LEFT && enableHijack)
    {
        if (useCam)
            isChange = true; // �G���v���C���[
        else
            isHijack = true; // �v���C���[���J�����������ăn�C�W���b�N������

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
    /// �G�ƐڐG�����ꍇ�̓v���C���[���_�ɖ߂�
    if (isHit)useCam = false;

    enableHijack = true;
    if (enableHijackTime < 8.0f && !useCam)
        enableHijack = false;

    // ���E�ύX�Ɉ�萔�̃Q�[�W�̌���
    if (isHijack)
    {
        // ��萔�̃Q�[�W����
        enableHijackTime -= hijackCost;
    }

    // �J�������n�C�W���b�N���Ă���ꍇ
    if (useCam)
    {
        // �Q�[�W�̏���
        //enableHijackTime -= hijackCostPerSec * dt;
    }
    // ���E���v���C���[�̏ꍇ
    else
    {
        // �n�C�W���b�N�ł��鎞�Ԃ��n�C�W���b�N�ł���ő厞�Ԃ�菬�����ꍇ
        if (maxHijackTime > enableHijackTime)
        {
            // �Q�[�W�̉�
            enableHijackTime += hijackRecoveryPerSec * dt;

            // �Q�[�W�̐���
            if (enableHijackTime > maxHijackTime)
                enableHijackTime = maxHijackTime;
        }
    }
}

// �A�j���[�V�����X�V����
void Player::UpdateAnimation(float dt)
{
    if (!model->GetResource()->GetAnimations().empty())
        animationController.UpdateAnimation(dt);
}

void Player::DeathState(float dt)
{
    DirectX::XMFLOAT3 enemyPos = enemyRef->GetPosition();
    DirectX::XMVECTOR EnemyPos, PlayerPos, PlayerToEnemyDir;

    EnemyPos = DirectX::XMLoadFloat3(&enemyPos);
    PlayerPos = DirectX::XMLoadFloat3(&position);
    PlayerToEnemyDir = DirectX::XMVectorSubtract(EnemyPos, PlayerPos);
    PlayerToEnemyDir = DirectX::XMVector3Normalize(PlayerToEnemyDir);

    float x, y, z;
    DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&world);
    DirectX::XMVECTOR Forward = M.r[2];

    x = DirectX::XMVectorGetX(Forward);
    z = DirectX::XMVectorGetZ(Forward);

    y = DirectX::XMVectorGetY(Forward);

    pitch = asinf(y);             // �㉺�̌���
    yaw = atan2f(x, z);           // ���E�̌���

    //// �p�x�����߂�
    //{
    //    //DirectX::XMFLOAT3 front = { 0,0,1 };
    //    DirectX::XMVECTOR Front, PlayerDir;
    //    //Front = DirectX::XMLoadFloat3(&front);
    //    //PlayerDir = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&forward));

    //    PlayerDir = 

    //    DirectX::XMVECTOR Dot, Cross;
    //    DirectX::XMFLOAT3 crossVector;
    //    float dot;
    //    Dot = DirectX::XMVector3Dot(PlayerDir, PlayerToEnemyDir);
    //    Cross = DirectX::XMVector3Cross(PlayerDir, PlayerToEnemyDir);
    //    DirectX::XMStoreFloat(&dot, Dot);
    //    DirectX::XMStoreFloat3(&crossVector, Cross);

    //    float radian = acosf(dot);

    //    if (crossVector.y < 0)
    //        radian *= -1;

    //    angle.y = radian;
    //}
}
