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
    Player(const DirectX::XMFLOAT3& position = {0,0,0});
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

    void SetEnableOpenGate(bool flag) { enableOpenGate = flag; }

    bool GetIsDeath() const { return isDeath; } ///< �v���C���[�����S�������ǂ����̔���

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

    bool enableOpenGate = false; ///< �h�A��������邩�ǂ��� ���̃t���O��false�̏ꍇ�J�����̐؂�ւ��\�Atrue�̏ꍇ�̓J�����؂�ւ��s��
    bool isDeath = false;
};

