#include "Player.h"
#include "System/Input.h"
#include "Camera/Camera.h"
#include "imgui.h"
#include <algorithm>
#include "Math/Easing.h"

static bool hit = false;
static float time = 0;
static constexpr float totalTime = 1;
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
	//model = std::make_unique<Model>("./Data/Model/Player/player.mdl");
	model = std::make_unique<Model>("./Data/Model/Player/player_mesh.mdl");
#endif

    // プレイヤーのパラメータ初期設定
    {
        position = { 1,0,-24 };
        scale = { 0.015, 0.015, 0.015 };    // スケール
        viewPoint = 1.5;                    // カメラの目線を設定するため
        radius = 0.6;                         // デバッグ用
        enableHijackTime = maxHijackTime;   // ハイジャックできる時間の設定
        acceleration = 1.1f;
        deceleration = 1.2f;
        hit = false;
        time = 0;
    }

    /// アニメーション関係設定
    {
        animationController.SetModel(model);
        animationController.PlayAnimation(static_cast<int>(AnimationState::MOVE), true);
        animationController.SetAnimationSecondScale(1.0f);
    }

    /// テクスチャの読み込み
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
	// ハイジャックの時間処理
	UpdateHijack(dt);

  // カメラ切り替え処理
  //ChangeCamera();

	// 移動処理
	Move(dt);

	if (isEvent) ///< Move() の中でフラグ切り替えをしてる
		DeathState(dt);

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

	/// テクスチャのセット
	textures->Set(rc);

	/// モデルがあるときかつ、プレイヤーが敵カメラを使っている場合
	/// プレイヤーを描画するとどうしても、モデルとカメラが被ってしまうので、
	/// 敵視点の時のみの描画にする
	if (model && useCam)
		renderer->Render(rc, world, model.get(), ShaderId::Custom);

	// テクスチャのクリア
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

		ImGui::Checkbox("isHit", &isHit);
	}
	ImGui::End();
}

// 移動処理
void Player::Move(float dt)
{
	if (!hit && isHit) ///< やりかたは汚いけど、一度ヒットしたらそれ以降はヒット判定にするために書く hit はPlayerコンストラクタの上でグローバルとしておいてる
	{
		accel = 0;
		hit = isHit;
	}

	/// 敵と接触した場合はだんだん速度を落として演出に入る
	if (hit)
	{
		if (speed > 0)
			accel -= deceleration * dt;
		else
			isEvent = true;
	}
	else
	{
		/// 加速処理
		accel += acceleration * dt;
	}

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

#if 0
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
	if (isHit)return;

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
	/// 敵と接触した場合はプレイヤー視点に戻す
	if (isHit)useCam = false;

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

		// �Q�[�W�̏���

		// �Q�[�W�̏���

		enableHijackTime -= hijackCostPerSec * dt;

		// ゲージの消費
		//enableHijackTime -= hijackCostPerSec * dt;

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
	if (!model->GetResource()->GetAnimations().empty())
		animationController.UpdateAnimation(dt);
}

void Player::DeathState(float dt)
{
    time += dt; ///< 演出用に使うタイマー
  
	DirectX::XMFLOAT3 enemyPos = {};
	if (enemyRef)
		enemyPos = enemyRef->GetPosition();

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

	pitch = asinf(y);             // 上下の向き
	yaw = atan2f(x, z);           // 左右の向き

  // 角度を求める
  {
        DirectX::XMVECTOR Dot, Cross;
        DirectX::XMFLOAT3 crossVector;
        float dot;
        Dot = DirectX::XMVector3Dot(Forward, PlayerToEnemyDir);
        Cross = DirectX::XMVector3Cross(Forward, PlayerToEnemyDir);
        DirectX::XMStoreFloat(&dot, Dot);
        DirectX::XMStoreFloat3(&crossVector, Cross);

        //if (crossVector.y < 0)
        //    radian *= -1;
		float radian = acosf(dot);
    
        if (crossVector.y < 0)
        {
           if(radian > 0)
                radian *= -1;
            //if (angle.y > radian)
            //    angle.y -= 10 * 0.01745f * dt;

            //else
            //{
            //    
            //}

            //angle.y = radian;
        }
        //else
        //{
        //    if (angle.y < radian)
        //        angle.y += 10 * 0.01745f * dt;
        //}
        time = DirectX::XMMin(time, totalTime);

        static float angleX = angle.x;
        static float angleY = angle.y;

        if(angle.x < 0)
            angle.x = Easing::InSine(time, totalTime, 0.0f, angleX);
        else
            angle.x = Easing::InSine(time, totalTime, angleX, 0.0f);

        angle.y = Easing::OutBack(time, totalTime, 1.0f, angleY, radian);
    }
}
