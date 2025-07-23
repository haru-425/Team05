#pragma once
#include "GameObject.h"
#include <memory>
#include "Enemy/Enemy.h"
#include "System/AnimationController.h"
#include "System/LoadTextures.h"

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
    Player();
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

    // テスト用なので気にしないで下さい
#pragma region テスト用
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
        //スケール行列を作成
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(t_scale.x, t_scale.y, t_scale.x);
        //回転行列
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(t_angle.x, t_angle.y, t_angle.z);
        //位置行列を作成
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(t_position.x, t_position.y, t_position.z);
        //３つの行列を組み合わせ、ワールド行列を作成
        DirectX::XMMATRIX W = S * R * T;
        //計算したワールド行列を取り出す
        DirectX::XMStoreFloat4x4(&t_transform, W);
    }
#endif
#pragma endregion
};

