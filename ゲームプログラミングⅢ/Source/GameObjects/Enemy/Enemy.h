#pragma once
#include "GameObject.h"
#include <memory>
#include <DirectXCollision.h>
#include "System/Model.h"
#include "Stage.h"
#include "./System/AnimationController.h"
#include "System/LoadTextures.h"
#include "System/difficulty.h"
#include <memory>

/// �ʏ�ړ����x
#define USUAL_SPEED (1.05f*1.8f)
/// �v���C���[�ǐՎ��̈ړ����x
#define TRACKING_SPEED (5.0f*1.8f)
/// �C�z���������Ƃ��̈ړ����x
#define FEELING_SPEED (3.0f*1.8f)

#define EASY_SR 5.0f
#define NORMAL_AND_HARD_SR 10.0f

#define EASY_LR 15.0f
#define NORMAL_AND_HARD_LR 30.0f

class Player;

/**
 * @brief �G�L�����N�^�[��\���N���X
 *
 * �v���C���[�̒ǐՂ�o�H�ړ��A���E���o�Ȃǂ̏������Ǘ�����B
 */
class Enemy : public GameObject
{
public:
    /// �f�t�H���g�R���X�g���N�^
    Enemy() {}

    /**
     * @brief �v���C���[�Q�ƂƃX�e�[�W�Q�Ƃ�n���ď���������R���X�g���N�^
     * @param playerRef �v���C���[�ւ̋��L�Q��
     * @param stage �X�e�[�W�ւ̃|�C���^
     */
    Enemy(std::shared_ptr<Player> playerRef, Stage* stage);

    /// �f�X�g���N�^
    ~Enemy();

    /**
     * @brief �X�V����
     * @param dt �o�ߎ��ԁi�b�j
     */
    void Update(float dt) override;

    /**
     * @brief ���f���̕`��
     * @param rc �`��R���e�L�X�g
     * @param renderer ���f�������_���[
     */
    void Render(const RenderContext& rc, ModelRenderer* renderer);

    /**
     * @brief �v���C���[�Q�Ƃ�ݒ肷��
     * @param player �v���C���[�̋��L�Q��
     */
    void SetPlayer(std::shared_ptr<Player> player) { playerRef = player; }

    /**
    * @brief �G�̓�Փx��ݒ肷��
    */
    void SetDifficulty();

    /**
     * @brief �o�H�ɐV�����|�C���g��ǉ�����
     * @param pos �ǉ����郏�[���h���W
     */
    void Addroute(DirectX::XMFLOAT3 pos) { route.push_back(pos); }

    /**
     * @brief �ړ��������X�V����
     * @param elapsedTime �o�ߎ��ԁi�b�j
     */
    void Updatemovement(float elapsedTime);

    /// �f�o�b�O�p�̉�������
    void DrawDebug() override;

    /// ���݂̍��W���擾����
    DirectX::XMFLOAT3 GetPosition() const { return position; }

    /// ���݂̃s�b�`�p���擾����
    float GetPitch() const { return pitch; }

    /// ���݂̃��[�p���擾����
    float GetYaw() const { return yaw; }

    void JageDirection(DirectX::XMVECTOR dir);

    void Animationplay();

    /// ���݌������Ă���o�H�|�C���g�̃C���f�b�N�X�i�f�o�b�O�p�j
    int GetindexWayPoint() const { return static_cast<int>(currentTargetIndex); }

    bool GetIsDead() const { return isDead; }

    void remote_sensing(DirectX::XMFLOAT3 pos);

    bool Get_Tracking() const { return isTrackingPlayer; }

    bool Get_isPlayerInView() const { return isPlayerInView; } // isPlayerInView

    bool Get_Loocking() const { return loocking; } 

private:
    /**
     * @brief �o�H�̍č\�����s���i�␳�p�j
     * @param start �J�n�C���f�b�N�X
     * @param current ���݃C���f�b�N�X
     */
    void refinePath(int start, int current);

private:
    /**
     * @brief �G�̏�Ԃ������񋓌^
     */
    enum class State
    {
        Idle,       ///< �ҋ@���
        Roaming,    ///< ����
        detection,  ///< �v���C���[����
        feeling,    ///< �C�z���m
        miss,       ///< ������
        turn,       ///< ��]
        Attack,     ///< �U��
    };

private:
    std::shared_ptr<Model> model = nullptr;                ///< ���f���f�[�^
    std::shared_ptr<LoadTextures> textures;                /// �e�N�X�`���f�[�^
    std::weak_ptr<Player> playerRef;                       ///< �v���C���[�ւ̎�Q��
    Stage* stage;                                          ///< �X�e�[�W�ւ̎Q��

    State state = State::Roaming;                          ///< ���݂̏��
    std::vector<DirectX::XMFLOAT3> route;                  ///< �ړ����[�g
    size_t currentTargetIndex = 0;                         ///< ���݌������Ă���|�C���g�̃C���f�b�N�X
    DirectX::XMFLOAT3 targetPosition = { 0, 0, 0 };        ///< ���݂̃^�[�Q�b�g���W
    float attackRange = 2.0f;                              ///< �U���͈�
    float moveSpeed = USUAL_SPEED;                         ///< ���݂̈ړ����x
    float turnSpeed = DirectX::XMConvertToRadians(360);    ///< ��]���x�i���W�A���j

    float stateTimer = 0.0f;                               ///< ��ԑJ�ڗp�̃^�C�}�[

    float pitch = 0;                                       ///< �s�b�`�p
    float yaw = 0;                                         ///< ���[�p

    /// �T���p�̃t���O��
    bool loocking;                                         ///< ���Ă��邩�ǂ���
    bool isTrackingPlayer = false;                         ///< �v���C���[��ǐՒ���
    bool isPlayerInView = false;
    bool isReverseTraced = false;                          ///< �v���C���[���t�T�m�����ォ�ǂ���
    float hitdist;                                         ///< �Փ˂܂ł̋���
    float searchRange = 10.0f;                             ///< �v���C���[�T���͈�
    float lockonRange = 80.0f;                             ///< �����Ńv���C���[��������鋗��

    //�A�j���[�V�����Ǘ��t���O�E�ϐ�
    AnimationController animationcontroller;
    float animation_Timer = 0.0f;
    enum Direction
    {
        N,
        S,
        W,
        E,
    };
    Direction direction = Direction::N;
    Direction olddirection = direction;

    DirectX::XMFLOAT2 cameraShakeScale = { 0.01f, 0.01f };

    bool isDead = false;
};
