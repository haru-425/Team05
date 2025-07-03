#include "Player.h"
#include "System/Input.h"
#include "Camera/Camera.h"
#include "imgui.h"

Player::Player()
{
#ifdef TEST
    // 確認用
    model = std::make_shared<Model>("./Data/Model/Test/test_walk_animation_model.mdl");
    t_position.x += 0.2f;
    t_position.z += 0.5f;
    t_position.y = 1.15f;
    t_scale = { 0.025,0.025,0.025 };

#else
    // 実際に使うモデル
    model = std::make_unique<Model>("./Data/Model/")
#endif

    // プレイヤーのパラメータ初期設定
    {
        viewPoint = 1.5;                    // カメラの目線を設定するため
        radius = 2;                         // デバッグ用
        enableHijackTime = maxHijackTime;   // ハイジャックできる時間の設定
    }
    animationController.SetModel(model);

    animationController.PlayAnimation(static_cast<int>(AnimationState::MOVE), true);
    animationController.SetAnimationSecondScale(5.0f);
}

Player::~Player()
{
}

void Player::Update(float dt)
{
    // ハイジャックの時間処理
    UpdateHijack(dt);

    // カメラ切り替え処理
    ChangeCamera();

    // 移動処理
    Move(dt);

#ifdef TEST
    TestTransformUpdate();
#endif
    // シーンの最後にもすることにします
    // プレイヤーの行動、行列更新
    // ↓
    // コリジョン
    // ↓
    // 行列更新
    // 
    // 行列更新処理
    UpdateTransform();

    // アニメーションの更新処理
    UpdateAnimation(dt);

    // モデルの行列更新処理
    model->UpdateTransform();
}

// 描画処理
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

// デバッグ描画処理
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

// 移動処理
void Player::Move(float dt)
{
    Camera& cam = Camera::Instance();

    DirectX::XMFLOAT3 forward;
    // カメラが切り替わっていないときだけカメラの方向を取る
    if (!useCam)
    {
        forward = cam.GetFront();
    }
    else
        forward = saveDirection;

    // カメラの方向をXZ面に固定
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

    // 角度を求める
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

// カメラ切り替え処理、実際のカメラ切り替えは外部でする
void Player::ChangeCamera()
{
    Mouse& mouse = Input::Instance().GetMouse();

    if (isChange)isChange = false; // 一回だけ通したい
    if (isHijack)isHijack = false;

    // カメラ切り替え
    if (mouse.GetButtonDown() & Mouse::BTN_LEFT && enableHijack)
    {
        if (useCam)
            isChange = true; // 敵→プレイヤー
        else
            isHijack = true; // プレイヤーがカメラを持ってハイジャックしたか

        useCam = !useCam;
    }

    // ゲージがなくなると強制的に戻す
    if (enableHijackTime <= 0 && useCam)
    {
        useCam = false;
        isChange = true;
    }
}

// ハイジャックゲージの更新処理
void Player::UpdateHijack(float dt)
{

    enableHijack = true;
    if (enableHijackTime < 8.0f && !useCam)
        enableHijack = false;

    // 視界変更に一定数のゲージの減り
    if (isHijack)
    {
        // 一定数のゲージ消費
        enableHijackTime -= hijackCost;
    }

    // カメラをハイジャックしている場合
    if (useCam)
    {
        // ゲージの消費
        enableHijackTime -= hijackCostPerSec * dt;
    }
    // 視界がプレイヤーの場合
    else
    {
        // ハイジャックできる時間がハイジャックできる最大時間より小さい場合
        if (maxHijackTime > enableHijackTime)
        {
            // ゲージの回復
            enableHijackTime += hijackRecoveryPerSec * dt;

            // ゲージの制限
            if (enableHijackTime > maxHijackTime)
                enableHijackTime = maxHijackTime;
        }
    }
}

// アニメーション更新処理
void Player::UpdateAnimation(float dt)
{
    animationController.UpdateAnimation(dt);
}
