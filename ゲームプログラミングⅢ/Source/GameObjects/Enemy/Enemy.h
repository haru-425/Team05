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

/// 通常移動速度
#define USUAL_SPEED (1.05f*1.8f)
/// プレイヤー追跡時の移動速度
#define TRACKING_SPEED (5.0f*1.8f)
/// 気配を感じたときの移動速度
#define FEELING_SPEED (3.0f*1.8f)

#define EASY_SR 5.0f
#define NORMAL_AND_HARD_SR 10.0f

#define EASY_LR 15.0f
#define NORMAL_AND_HARD_LR 30.0f

class Player;

/**
 * @brief 敵キャラクターを表すクラス
 *
 * プレイヤーの追跡や経路移動、視界検出などの処理を管理する。
 */
class Enemy : public GameObject
{
public:
    /// デフォルトコンストラクタ
    Enemy() {}

    /**
     * @brief プレイヤー参照とステージ参照を渡して初期化するコンストラクタ
     * @param playerRef プレイヤーへの共有参照
     * @param stage ステージへのポインタ
     */
    Enemy(std::shared_ptr<Player> playerRef, Stage* stage);

    /// デストラクタ
    ~Enemy();

    /**
     * @brief 更新処理
     * @param dt 経過時間（秒）
     */
    void Update(float dt) override;

    /**
     * @brief モデルの描画
     * @param rc 描画コンテキスト
     * @param renderer モデルレンダラー
     */
    void Render(const RenderContext& rc, ModelRenderer* renderer);

    /**
     * @brief プレイヤー参照を設定する
     * @param player プレイヤーの共有参照
     */
    void SetPlayer(std::shared_ptr<Player> player) { playerRef = player; }

    /**
    * @brief 敵の難易度を設定する
    */
    void SetDifficulty();

    /**
     * @brief 経路に新しいポイントを追加する
     * @param pos 追加するワールド座標
     */
    void Addroute(DirectX::XMFLOAT3 pos) { route.push_back(pos); }

    /**
     * @brief 移動処理を更新する
     * @param elapsedTime 経過時間（秒）
     */
    void Updatemovement(float elapsedTime);

    /// デバッグ用の可視化処理
    void DrawDebug() override;

    /// 現在の座標を取得する
    DirectX::XMFLOAT3 GetPosition() const { return position; }

    /// 現在のピッチ角を取得する
    float GetPitch() const { return pitch; }

    /// 現在のヨー角を取得する
    float GetYaw() const { return yaw; }

    void JageDirection(DirectX::XMVECTOR dir);

    void Animationplay();

    /// 現在向かっている経路ポイントのインデックス（デバッグ用）
    int GetindexWayPoint() const { return static_cast<int>(currentTargetIndex); }

    bool GetIsDead() const { return isDead; }

    void remote_sensing(DirectX::XMFLOAT3 pos);

    bool Get_Tracking() const { return isTrackingPlayer; }

    bool Get_isPlayerInView() const { return isPlayerInView; } // isPlayerInView

    bool Get_Loocking() const { return loocking; } 

private:
    /**
     * @brief 経路の再構成を行う（補正用）
     * @param start 開始インデックス
     * @param current 現在インデックス
     */
    void refinePath(int start, int current);

private:
    /**
     * @brief 敵の状態を示す列挙型
     */
    enum class State
    {
        Idle,       ///< 待機状態
        Roaming,    ///< 巡回中
        detection,  ///< プレイヤー発見
        feeling,    ///< 気配感知
        miss,       ///< 見失い
        turn,       ///< 回転
        Attack,     ///< 攻撃
    };

private:
    std::shared_ptr<Model> model = nullptr;                ///< モデルデータ
    std::shared_ptr<LoadTextures> textures;                /// テクスチャデータ
    std::weak_ptr<Player> playerRef;                       ///< プレイヤーへの弱参照
    Stage* stage;                                          ///< ステージへの参照

    State state = State::Roaming;                          ///< 現在の状態
    std::vector<DirectX::XMFLOAT3> route;                  ///< 移動ルート
    size_t currentTargetIndex = 0;                         ///< 現在向かっているポイントのインデックス
    DirectX::XMFLOAT3 targetPosition = { 0, 0, 0 };        ///< 現在のターゲット座標
    float attackRange = 2.0f;                              ///< 攻撃範囲
    float moveSpeed = USUAL_SPEED;                         ///< 現在の移動速度
    float turnSpeed = DirectX::XMConvertToRadians(360);    ///< 回転速度（ラジアン）

    float stateTimer = 0.0f;                               ///< 状態遷移用のタイマー

    float pitch = 0;                                       ///< ピッチ角
    float yaw = 0;                                         ///< ヨー角

    /// 探索用のフラグ等
    bool loocking;                                         ///< 見ているかどうか
    bool isTrackingPlayer = false;                         ///< プレイヤーを追跡中か
    bool isPlayerInView = false;
    bool isReverseTraced = false;                          ///< プレイヤーを逆探知した後かどうか
    float hitdist;                                         ///< 衝突までの距離
    float searchRange = 10.0f;                             ///< プレイヤー探索範囲
    float lockonRange = 80.0f;                             ///< 直線でプレイヤーを見つけれる距離

    //アニメーション管理フラグ・変数
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
