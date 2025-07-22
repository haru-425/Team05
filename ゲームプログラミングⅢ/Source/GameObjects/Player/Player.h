#pragma once
#include "GameObject.h"
#include <memory>
#include "Enemy/Enemy.h"
#include "System/AnimationController.h"
#include "System/LoadTextures.h"

// �e�X�g�p�̃��f�����������؂�ւ���悤
//#define TEST

// �v���C���[�p�����[�^�ݒ�
static constexpr float maxHijackTime        = 50; // �n�C�W���b�N�̍ő厞��
static constexpr int hijackCost             = 5;   // �n�C�W���b�N�R�X�g
static constexpr int hijackCostPerSec       = 5;   // ��b���Ƃ̃n�C�W���b�N�R�X�g
static constexpr int hijackRecoveryPerSec   = 3;   // ��b���Ƃ̃n�C�W���b�N�R�X�g�̉񕜗�
static constexpr float maxSpeed             = 3.0f; // �v���C���[�̍ō����x
static float acceleration                   = 1.1f; // �����x

static enum class AnimationState
{
    MOVE
};

class Player : public GameObject
{
public:
    Player();
    ~Player();

    // �X�V����
    void Update(float dt) override;

    // �`�揈��
    void Render(const RenderContext& rc, ModelRenderer* renderer) override;

    void DrawDebug() override;

    // �G���o�C���h
    void SetEnemy(std::shared_ptr<Enemy> enemy) { enemyRef = enemy; }

    bool GetUseCam() const { return useCam; }

    bool GetIsChange() const { return isChange; }

    DirectX::XMFLOAT3 GetDirection() const { return saveDirection; }

    /// ���S���o�p�t���O�̃Q�b�^�[
    bool GetIsEvent() const { return isEvent; }

    float GetPitch() const {    return pitch;}
    float GetYaw() const { return yaw; }

    void AddHijackTimer(float timer) {

        enableHijackTime += timer;
        if (enableHijackTime > maxHijackTime)
            enableHijackTime = maxHijackTime;
    }

private:
    void Move(float dt);

    void ChangeCamera();

    void UpdateHijack(float dt);

    void UpdateAnimation(float dt);

    void DeathState(float dt); ///< ���S���o�p

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<Enemy> enemyRef = nullptr; // �G�p

    float accel         = 0.0f; // �����x
    float speed         = 0.0f;  // �v���C���[�ړ����x
    bool useCam         = false; // true : �G���_ ,false : �v���C���[���_
    bool isChange       = false; // �J������ς������ǂ���
    bool isHijack       = false; // �G�̎��E���n�C�W���b�N�����̂� �����g���ĂȂ�
    bool enableHijack   = false; // �n�C�W���b�N�ł���̂�
    bool isEvent        = false; // ���S���o�t���O

    float enableHijackTime; // �n�C�W���b�N����
    float hijackSpeed = 0.0f;  // ���E�W���b�N�̎��Ԃ����炷���x

    float pitch;
    float yaw;
    float angleX; ///< ���S���o�p   
    float angleY; ///< ���S���o�p   

    AnimationController animationController; // �A�j���[�V����
    AnimationState state = AnimationState::MOVE;

    std::unique_ptr<LoadTextures> textures;  // �e�N�X�`��

    // �e�X�g�p�Ȃ̂ŋC�ɂ��Ȃ��ŉ�����
#pragma region �e�X�g�p
#if defined TEST
    DirectX::XMFLOAT3 t_position = { 0,0,0 };
    DirectX::XMFLOAT3 t_angle = { 0,0,0 };
    DirectX::XMFLOAT3 t_scale = { 1,1,1 };
    DirectX::XMFLOAT4X4 t_transform = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };

    void TestTransformUpdate()
    {
        //�X�P�[���s����쐬
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(t_scale.x, t_scale.y, t_scale.x);
        //��]�s��
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(t_angle.x, t_angle.y, t_angle.z);
        //�ʒu�s����쐬
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(t_position.x, t_position.y, t_position.z);
        //�R�̍s���g�ݍ��킹�A���[���h�s����쐬
        DirectX::XMMATRIX W = S * R * T;
        //�v�Z�������[���h�s������o��
        DirectX::XMStoreFloat4x4(&t_transform, W);
    }
#endif
#pragma endregion
};

