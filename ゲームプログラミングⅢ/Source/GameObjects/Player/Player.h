#pragma once
#include "GameObject.h"
#include <memory>
#include "Enemy/Enemy.h"
#include "System/AnimationController.h"
#include "System/LoadTextures.h"
#include "System/AudioSource.h"

// テスト用のモデルだったりを切り替えるよう
//#define TEST

// プレイヤーパラメータ設定
static constexpr float maxHijackTime        = 50; // ハイジャックの最大時間
static constexpr int hijackCost             = 5;   // ハイジャックコスト
static constexpr int hijackCostPerSec       = 5;   // 一秒ごとのハイジャックコスト
static constexpr int hijackRecoveryPerSec   = 3;   // 一秒ごとのハイジャックコストの回復量
static constexpr float maxSpeed             = 3.0f; // プレイヤーの最高速度
static float acceleration                   = 1.1f; // 加速度

static enum class AnimationState
{
    MOVE
};

class Player : public GameObject
{
public:
    Player(const DirectX::XMFLOAT3& position = {0,0,0});
    ~Player();

    // 更新処理
    void Update(float dt) override;

    // 描画処理
    void Render(const RenderContext& rc, ModelRenderer* renderer) override;

    void DrawDebug() override;

    // 敵をバインド
    void SetEnemy(std::shared_ptr<Enemy> enemy) { enemyRef = enemy; }

    bool GetUseCam() const { return useCam; }

    bool GetIsChange() const { return isChange; }

    DirectX::XMFLOAT3 GetDirection() const { return saveDirection; }

    /// 死亡演出用フラグのゲッター
    bool GetIsEvent() const { return isEvent; }

    float GetPitch() const {    return pitch;}
    float GetYaw() const { return yaw; }

    void AddHijackTimer(float timer) {

        enableHijackTime += timer;
        if (enableHijackTime > maxHijackTime)
            enableHijackTime = maxHijackTime;
    }

    void SetEnableOpenGate(bool flag) { enableOpenGate = flag; }

    bool GetIsDeath() const { return isDeath; } ///< プレイヤーが死亡したかどうかの判定

    float GetenableHijackTime(){ return enableHijackTime;}

    void DeleteSounds();

private:
    void Move(float dt);

    void ChangeCamera();

    void UpdateHijack(float dt);

    void UpdateAnimation(float dt);

    void DeathState(float dt); ///< 死亡演出用


private:
    std::shared_ptr<Model> model;
    std::shared_ptr<Enemy> enemyRef = nullptr; // 敵用

    float accel         = 0.0f; // 加速度
    float speed         = 0.0f;  // プレイヤー移動速度
    bool useCam         = false; // true : 敵視点 ,false : プレイヤー視点
    bool isChange       = false; // カメラを変えたかどうか
    bool isHijack       = false; // 敵の視界をハイジャックしたのか 多分使ってない
    bool enableHijack   = false; // ハイジャックできるのか
    bool isEvent        = false; // 死亡演出フラグ

    float enableHijackTime; // ハイジャック時間
    float hijackSpeed = 0.0f;  // 視界ジャックの時間を減らす速度

    float pitch;
    float yaw;
    float angleX; ///< 死亡演出用   
    float angleY; ///< 死亡演出用   

    AnimationController animationController; // アニメーション
    AnimationState state = AnimationState::MOVE;

    std::unique_ptr<LoadTextures> textures;  // テクスチャ

    bool enableOpenGate = false; ///< ドアをくぐれるかどうか このフラグがfalseの場合カメラの切り替え可能、trueの場合はカメラ切り替え不可
    bool isDeath = false;

    // カメラ切り替えのSE
    AudioSource* changeCameraInSE = nullptr;
    AudioSource* changeCameraKeepSE = nullptr;
};

