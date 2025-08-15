#pragma once

#include "GameObject.h"
#include <memory>
#include <DirectXCollision.h>
#include "System/Model.h"
#include "Stage.h"
#include "./System/AnimationController.h"
#include "System/LoadTextures.h"
#include "System/difficulty.h"

#define USUAL_SPEED (1.05f * 1.8f)        // �ʏ�ړ����x
#define TRACKING_SPEED (5.0f * 1.8f)      // �v���C���[�ǐՎ��̑��x
#define FEELING_SPEED (3.0f * 1.8f)       // �C�z���������Ƃ��̑��x

#define EASY_SR 5.0f                      // �C�[�W�[�̒T�������i�Z�j
#define NORMAL_AND_HARD_SR 10.0f         // �m�[�}���E�n�[�h�̒T�������i�Z�j

#define EASY_LR 15.0f                     // �C�[�W�[�̎��F�����i���j
#define NORMAL_AND_HARD_LR 20.0f         // �m�[�}���E�n�[�h�̎��F�����i���j

class Player;

// �G�L�����N�^�[�N���X�i�v���C���[���m�E�ǐՁE�o�H�ړ��Ȃǂ������j
class Enemy : public GameObject
{
public:
    Enemy() {}

    // �R���X�g���N�^�F�v���C���[�ƃX�e�[�W�̎Q�Ƃ��󂯎��
    Enemy(std::shared_ptr<Player> playerRef, Stage* stage);

    ~Enemy();

    // �X�V�����i�e��Ԃɉ����������j
    void Update(float dt) override;

    // ���f���̕`��
    void Render(const RenderContext& rc, ModelRenderer* renderer);

    // �v���C���[�Q�Ƃ��O������Z�b�g
    void SetPlayer(std::shared_ptr<Player> player) { playerRef = player; }

    // �G�̓�Փx�ɉ����Ċe��p�����[�^����
    void SetDifficulty();

    // �o�H�|�C���g��ǉ��i�ړ���o�^�j
    void Addroute(DirectX::XMFLOAT3 pos) { route.push_back(pos); }

    // ����E�ǐՂȂǂ̈ړ�����
    void Updatemovement(float elapsedTime);

    // �f�o�b�O�`��i�������j
    void DrawDebug() override;

    // ���݂̃��[���h���W
    DirectX::XMFLOAT3 GetPosition() const { return position; }

    // �����Ă���s�b�`�i�㉺�j
    float GetPitch() const { return pitch; }

    // �����Ă��郈�[�i���E�j
    float GetYaw() const { return yaw; }

    // �w������������⏕�֐�
    void JageDirection(DirectX::XMVECTOR dir);

    // �A�j���[�V�����X�V����
    void Animationplay();

    // ���݂̌o�H�C���f�b�N�X�i���Ԗڂ̃|�C���g�Ɍ������Ă��邩�j
    int GetindexWayPoint() const { return static_cast<int>(currentTargetIndex); }

    // �G�����S���Ă��邩�ǂ���
    bool GetIsDead() const { return isDead; }

    // ���m�i����C�z�j�����F�w��ʒu�ɑ΂��Ċ��m��������
    void remote_sensing(DirectX::XMFLOAT3 pos);

    // �v���C���[��ǐՒ���
    bool Get_Tracking() const { return isTrackingPlayer; }

    // �v���C���[�����E���ɂ��邩
    bool Get_isPlayerInView() const { return isPlayerInView; }

    // �G���u�������Ă���v��Ԃ��i�^�[�Q�b�g�����Ă���j
    bool Get_Loocking() const { return loocking; }
    //�|�[�Y��Ԃ��畜�A����Ƃ��̃T�E���h����
    void play_Enemy_Sound();

private:
    // �o�H�č\�z�i���ݒn�_����S�[���܂ł��t���ɂ��ǂ��Đ����ɂ���j
    void refinePath(int start, int current);

    // ��ԁiAI�ɂ��U�镑���؂�ւ��j
    enum class State
    {
        Idle,       // �ҋ@
        Roaming,    // ����i�o�H�ړ��j
        detection,  // ���F�������
        feeling,    // �C�z���m���
        miss,       // ������
        turn,       // �����]����
        Attack,     // �U����
    };

    // ���f�����
    std::shared_ptr<Model> model = nullptr;

    // �e�N�X�`���i���g�p�H�j
    std::shared_ptr<LoadTextures> textures;

    // �v���C���[�Q�Ɓi��Q�Ƃň��S�Ǘ��j
    std::weak_ptr<Player> playerRef;

    // �X�e�[�W�Q�Ɓi�|�C���^�j
    Stage* stage;

    // ���݂̏��
    State state = State::Roaming;

    // �o�H�iWaypoint�ɂ��p�X�j
    std::vector<DirectX::XMFLOAT3> route;

    // ���݌������Ă���o�H�|�C���g�̃C���f�b�N�X
    size_t currentTargetIndex = 0;

    // ���݂̃^�[�Q�b�g���W�i�ړ���j
    DirectX::XMFLOAT3 targetPosition = { 0, 0, 0 };

    float attackRange = 2.0f;           // �U���\�ȋ���
    float moveSpeed = USUAL_SPEED;      // ���݂̈ړ����x
    float turnSpeed = DirectX::XMConvertToRadians(360); // ��]���x�i���b���W�A���j

    float stateTimer = 0.0f;           // ��Ԉێ��^�C�}�[�i��莞�Ԍ�ɏ�ԑJ�ځj

    float pitch = 0;                   // �s�b�`�p�i�㉺�����j
    float yaw = 0;                     // ���[�p�i���E�����j

    // ���m�E�ǐՊ֘A�t���O
    bool loocking = false;            // �^�[�Q�b�g�𒍎�����
    bool isTrackingPlayer = false;    // �v���C���[��ǐՂ��Ă��邩
    bool isPlayerInView = false;      // ���E�Ƀv���C���[�����邩
    bool isReverseTraced = false;     // �t�T�m���i�v���C���[�̈ʒu���@�m��̕⑫�j
    float hitdist = 0.0f;             // �v���C���[�Ƃ̋����i�Փ˗p�j

    // �T���֘A
    float searchRange = 10.0f;        // �T���͈́i���m�����j
    float lockonRange = 30.0f;        // ���F�����i���b�N�I���ł��鋗���j

    // �A�j���[�V��������
    AnimationController animationcontroller;
    float animation_Timer = 0.0f;

    // �G�̌����i�k�쓌���j
    enum Direction
    {
        N, // �k
        S, // ��
        W, // ��
        E  // ��
    };
    Direction direction = Direction::N;
    Direction olddirection = direction;

    // �J�����V�F�C�N�i�U�����Ȃǂɗh�炷�����p�j
    DirectX::XMFLOAT2 cameraShakeScale = { 0.01f, 0.01f };

    // �G�����S�ς݂��ǂ���
    bool isDead = false;
};
