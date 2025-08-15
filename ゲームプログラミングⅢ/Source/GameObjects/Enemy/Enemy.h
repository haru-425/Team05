#pragma once

#include "GameObject.h"
#include <memory>
#include <DirectXCollision.h>
#include "System/Model.h"
#include "Stage.h"
#include "./System/AnimationController.h"
#include "System/LoadTextures.h"
#include "System/difficulty.h"

#define USUAL_SPEED (1.05f * 1.8f)        // 通常移動速度
#define TRACKING_SPEED (5.0f * 1.8f)      // プレイヤー追跡時の速度
#define FEELING_SPEED (3.0f * 1.8f)       // 気配を感じたときの速度

#define EASY_SR 5.0f                      // イージーの探索距離（短）
#define NORMAL_AND_HARD_SR 10.0f         // ノーマル・ハードの探索距離（短）

#define EASY_LR 15.0f                     // イージーの視認距離（長）
#define NORMAL_AND_HARD_LR 20.0f         // ノーマル・ハードの視認距離（長）

class Player;

// 敵キャラクタークラス（プレイヤー検知・追跡・経路移動などを扱う）
class Enemy : public GameObject
{
public:
    Enemy() {}

    // コンストラクタ：プレイヤーとステージの参照を受け取る
    Enemy(std::shared_ptr<Player> playerRef, Stage* stage);

    ~Enemy();

    // 更新処理（各状態に応じた処理）
    void Update(float dt) override;

    // モデルの描画
    void Render(const RenderContext& rc, ModelRenderer* renderer);

    // プレイヤー参照を外部からセット
    void SetPlayer(std::shared_ptr<Player> player) { playerRef = player; }

    // 敵の難易度に応じて各種パラメータ調整
    void SetDifficulty();

    // 経路ポイントを追加（移動先登録）
    void Addroute(DirectX::XMFLOAT3 pos) { route.push_back(pos); }

    // 巡回・追跡などの移動処理
    void Updatemovement(float elapsedTime);

    // デバッグ描画（未実装）
    void DrawDebug() override;

    // 現在のワールド座標
    DirectX::XMFLOAT3 GetPosition() const { return position; }

    // 向いているピッチ（上下）
    float GetPitch() const { return pitch; }

    // 向いているヨー（左右）
    float GetYaw() const { return yaw; }

    // 指定方向を向く補助関数
    void JageDirection(DirectX::XMVECTOR dir);

    // アニメーション更新処理
    void Animationplay();

    // 現在の経路インデックス（何番目のポイントに向かっているか）
    int GetindexWayPoint() const { return static_cast<int>(currentTargetIndex); }

    // 敵が死亡しているかどうか
    bool GetIsDead() const { return isDead; }

    // 感知（音や気配）処理：指定位置に対して感知反応する
    void remote_sensing(DirectX::XMFLOAT3 pos);

    // プレイヤーを追跡中か
    bool Get_Tracking() const { return isTrackingPlayer; }

    // プレイヤーが視界内にいるか
    bool Get_isPlayerInView() const { return isPlayerInView; }

    // 敵が「注視している」状態か（ターゲットを見ている）
    bool Get_Loocking() const { return loocking; }
    //ポーズ状態から復帰するときのサウンド処理
    void play_Enemy_Sound();

private:
    // 経路再構築（現在地点からゴールまでを逆順にたどって正順にする）
    void refinePath(int start, int current);

    // 状態（AIによる振る舞い切り替え）
    enum class State
    {
        Idle,       // 待機
        Roaming,    // 巡回（経路移動）
        detection,  // 視認発見状態
        feeling,    // 気配感知状態
        miss,       // 見失い
        turn,       // 方向転換中
        Attack,     // 攻撃中
    };

    // モデル情報
    std::shared_ptr<Model> model = nullptr;

    // テクスチャ（未使用？）
    std::shared_ptr<LoadTextures> textures;

    // プレイヤー参照（弱参照で安全管理）
    std::weak_ptr<Player> playerRef;

    // ステージ参照（ポインタ）
    Stage* stage;

    // 現在の状態
    State state = State::Roaming;

    // 経路（Waypointによるパス）
    std::vector<DirectX::XMFLOAT3> route;

    // 現在向かっている経路ポイントのインデックス
    size_t currentTargetIndex = 0;

    // 現在のターゲット座標（移動先）
    DirectX::XMFLOAT3 targetPosition = { 0, 0, 0 };

    float attackRange = 2.0f;           // 攻撃可能な距離
    float moveSpeed = USUAL_SPEED;      // 現在の移動速度
    float turnSpeed = DirectX::XMConvertToRadians(360); // 回転速度（毎秒ラジアン）

    float stateTimer = 0.0f;           // 状態維持タイマー（一定時間後に状態遷移）

    float pitch = 0;                   // ピッチ角（上下視線）
    float yaw = 0;                     // ヨー角（左右視線）

    // 感知・追跡関連フラグ
    bool loocking = false;            // ターゲットを注視中か
    bool isTrackingPlayer = false;    // プレイヤーを追跡しているか
    bool isPlayerInView = false;      // 視界にプレイヤーがいるか
    bool isReverseTraced = false;     // 逆探知中（プレイヤーの位置を察知後の補足）
    float hitdist = 0.0f;             // プレイヤーとの距離（衝突用）

    // 探索関連
    float searchRange = 10.0f;        // 探索範囲（感知距離）
    float lockonRange = 30.0f;        // 視認距離（ロックオンできる距離）

    // アニメーション制御
    AnimationController animationcontroller;
    float animation_Timer = 0.0f;

    // 敵の向き（北南東西）
    enum Direction
    {
        N, // 北
        S, // 南
        W, // 西
        E  // 東
    };
    Direction direction = Direction::N;
    Direction olddirection = direction;

    // カメラシェイク（攻撃時などに揺らす処理用）
    DirectX::XMFLOAT2 cameraShakeScale = { 0.01f, 0.01f };

    // 敵が死亡済みかどうか
    bool isDead = false;
};
