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
	// 実際に使ぁE��チE��
	//model = std::make_unique<Model>("./Data/Model/Player/player.mdl");
	model = std::make_unique<Model>("./Data/Model/Player/player_mesh.mdl");
#endif

    // プレイヤーのパラメータ初期設宁E
    {
        position = { 1,0,-24 };
        scale = { 0.015, 0.015, 0.015 };    // スケール
        viewPoint = 1.5;                    // カメラの目線を設定するためE
        radius = 0.6;                         // チE��チE��用
        enableHijackTime = maxHijackTime;   // ハイジャチE��できる時間の設宁E
        acceleration = 1.1f;
        deceleration = 1.2f;
        hit = false;
        time = 0;
    }

    /// アニメーション関係設宁E
    {
        animationController.SetModel(model);
        animationController.PlayAnimation(static_cast<int>(AnimationState::MOVE), true);
        animationController.SetAnimationSecondScale(1.0f);
    }

    /// チE��スチャの読み込み
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
	// ハイジャチE��の時間処琁E
	UpdateHijack(dt);

  // カメラ刁E��替え�E琁E
  //ChangeCamera();

	// 移動�E琁E
	Move(dt);

	if (isEvent) ///< Move() の中でフラグ刁E��替えをしてめE
		DeathState(dt);

#ifdef TEST
	TestTransformUpdate();
#endif
	// シーンの最後にもすることにしまぁE
	// プレイヤーの行動、行�E更新
	// ↁE
	// コリジョン
	// ↁE
	// 行�E更新
	// 
	// 行�E更新処琁E
	UpdateTransform();

	// アニメーションの更新処琁E
	UpdateAnimation(dt);

	// モチE��の行�E更新処琁E
	model->UpdateTransform();
}

// 描画処琁E
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
#ifndef TEST

	/// チE��スチャのセチE��
	textures->Set(rc);

	/// モチE��があるときかつ、�Eレイヤーが敵カメラを使ってぁE��場吁E
	/// プレイヤーを描画するとどぁE��ても、モチE��とカメラが被ってしまぁE�Eで、E
	/// 敵視点の時�Eみの描画にする
	if (model && useCam)
		renderer->Render(rc, world, model.get(), ShaderId::Custom);

	// チE��スチャのクリア
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

// チE��チE��描画処琁E
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

// 移動�E琁E
void Player::Move(float dt)
{
	if (!hit && isHit) ///< めE��かたは汚いけど、一度ヒットしたらそれ以降�Eヒット判定にするために書ぁEhit はPlayerコンストラクタの上でグローバルとしておいてめE
	{
		accel = 0;
		hit = isHit;
	}

	/// 敵と接触した場合�Eだんだん速度を落として演�Eに入めE
	if (hit)
	{
		if (speed > 0)
			accel -= deceleration * dt;
		else
			isEvent = true;
	}
	else
	{
		/// 加速�E琁E
		accel += acceleration * dt;
	}

	Camera& cam = Camera::Instance();

	DirectX::XMFLOAT3 forward;
	// カメラが�Eり替わってぁE��ぁE��きだけカメラの方向を取る
	if (!useCam)
	{
		forward = cam.GetFront();
	}
	else
		forward = saveDirection;

	// カメラの方向をXZ面に固宁E
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

	// 角度を求めめE
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

// カメラ刁E��替え�E琁E��実際のカメラ刁E��替え�E外部でする
void Player::ChangeCamera()
{
	if (isHit)return;

	Mouse& mouse = Input::Instance().GetMouse();

	if (isChange)isChange = false; // 一回だけ通したい
	if (isHijack)isHijack = false;

	// カメラ刁E��替ぁE
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT && enableHijack)
	{
		if (useCam)
			isChange = true; // 敵→�Eレイヤー
		else
			isHijack = true; // プレイヤーがカメラを持ってハイジャチE��したぁE

		useCam = !useCam;
	}

	// ゲージがなくなると強制皁E��戻ぁE
	if (enableHijackTime <= 0 && useCam)
	{
		useCam = false;
		isChange = true;
	}
}

// ハイジャチE��ゲージの更新処琁E
void Player::UpdateHijack(float dt)
{
	/// 敵と接触した場合�Eプレイヤー視点に戻ぁE
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

	// カメラをハイジャチE��してぁE��場吁E
	if (useCam)
	{

		// �Q�[�W�̏���

		// �E�Q�E�[�E�W�E�̏��E��E�

		enableHijackTime -= hijackCostPerSec * dt;

		// ゲージの消費
		//enableHijackTime -= hijackCostPerSec * dt;

	}
	// 視界が�Eレイヤーの場吁E
	else
	{
		// ハイジャチE��できる時間がハイジャチE��できる最大時間より小さぁE��吁E
		if (maxHijackTime > enableHijackTime)
		{
			// ゲージの回復
			enableHijackTime += hijackRecoveryPerSec * dt;

			// ゲージの制陁E
			if (enableHijackTime > maxHijackTime)
				enableHijackTime = maxHijackTime;
		}
	}
}

// アニメーション更新処琁E
void Player::UpdateAnimation(float dt)
{
	if (!model->GetResource()->GetAnimations().empty())
		animationController.UpdateAnimation(dt);
}

void Player::DeathState(float dt)
{
    time += dt; ///< 演�E用に使ぁE��イマ�E
  
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

	pitch = asinf(y);             // 上下�E向き
	yaw = atan2f(x, z);           // 左右の向き

  // 角度を求めめE
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
