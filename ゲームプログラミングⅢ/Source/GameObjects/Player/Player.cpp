#include "Player.h"
#include "System/Input.h"
#include "Camera/Camera.h"
#include "System/Audio.h"
#include "imgui.h"
#include <algorithm>
#include "Math/Easing.h"

static bool hit = false;
static float time = 0;
static constexpr float totalTime = 1;

Player::Player()
{
#ifdef TEST
	// チE��ト用モチE��の読み込み
	model = std::make_shared<Model>("./Data/Model/Test/test_walk_animation_model.mdl");
	t_position.x += 0.2f;
	t_position.z += 0.5f;
	t_position.y = 1.15f;
	t_scale = { 0.025,0.025,0.025 };
#else
	// 製品版プレイヤーモチE��の読み込み
	model = std::make_unique<Model>("./Data/Model/Player/player_mesh.mdl");
#endif

	// 初期パラメータの設宁E
	{
		position = { 1,0,-24 };
		scale = { 0.015, 0.015, 0.015 };
		viewPoint = 1.5;           // 視点の高さ
		radius = 0.6;              // 当たり判定半征E
		enableHijackTime = maxHijackTime;   // ハイジャチE��可能時間の初期値
		acceleration = 1.1f;
		deceleration = 1.2f;
		hit = false;
		time = 0;
	}

	{
		animationController.SetModel(model);
		animationController.PlayAnimation(static_cast<int>(AnimationState::MOVE), true);
		animationController.SetAnimationSecondScale(1.0f);
	}

	textures = std::make_unique<LoadTextures>();
	textures->LoadNormal("Data/Model/Player/Texture/player_mtl_Normal_DirectX.png");
	textures->LoadMetalness("Data/Model/Player/Texture/player_mtl_Metallic.png");
	textures->LoadEmisive("Data/Model/Player/Texture/player_mtl_Emissive.png");
	textures->LoadOcclusion("Data/Model/Player/Texture/player_mtl_Opacity.png");

    // SE�̓ǂݍ���
    changeCameraInSE = Audio::Instance().LoadAudioSource("Data/Sound/change_camera_in.wav");
    changeCameraKeepSE = Audio::Instance().LoadAudioSource("Data/Sound/change_camera_keep.wav");
}

Player::~Player()
{

}

void Player::Update(float dt)
{
	// ハイジャチE��関連の更新処琁E
	UpdateHijack(dt);

    // カメラ刁E��替え�E琁E
    if (changeCameraInSE->IsPlaying())
        changeCameraInSE->SetVolume(0.5f);

	// �J�����؂�ւ�����
	ChangeCamera();

	Move(dt);

	if (isEvent) // 移動中にイベント状態へ移衁E
		DeathState(dt);

#ifdef TEST
	TestTransformUpdate();
#endif
	// モチE��ワールド行�Eの更新
	UpdateTransform();

	// アニメーションの更新
	UpdateAnimation(dt);

	// モチE��にワールド行�Eを適用
	model->UpdateTransform();
}

// モチE��の描画
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
#ifndef TEST
	// チE��スチャ設宁E
	textures->Set(rc);

	// モチE��が存在し、カメラ刁E��替え中であれば描画
	if (model && useCam)
		renderer->Render(rc, world, model.get(), ShaderId::Custom);

	// チE��スチャの解除
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

// チE��チE��描画�E�EmGui�E�E
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

void Player::DeleteSounds()
{
	if (changeCameraInSE) {
		changeCameraInSE->Stop();
		delete changeCameraInSE;
		changeCameraInSE = nullptr;
	}
	if (changeCameraKeepSE) {
		changeCameraKeepSE->Stop();
		delete changeCameraKeepSE;
		changeCameraKeepSE = nullptr;
	}
}

// �v���C���[�ړ�����
void Player::Move(float dt)
{
	if (!hit && isHit) // 初めて攻撁E��受けた時の処琁E
	{
		accel = 0;
		hit = isHit;
	}

	// 減速�E琁E
	if (hit)
	{
		if (speed > 0)
			accel -= deceleration * dt;
		else
			isEvent = true;
	}
	else
	{
		// 加速�E琁E
		accel += acceleration * dt;
	}

	Camera& cam = Camera::Instance();

	DirectX::XMFLOAT3 forward;
	// 通常カメラ or ハイジャチE��視点
	if (!useCam)
	{
		forward = cam.GetFront();
	}
	else
		forward = saveDirection;

	// XZ 平面の方向�Eクトルに正規化
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

	// プレイヤーの向き計算！E軸回転�E�E
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

// カメラの刁E��替え�E琁E��左クリチE���E�E
void Player::ChangeCamera()
{
	if (isHit)return;

	Mouse& mouse = Input::Instance().GetMouse();

	if (isChange)isChange = false;
	if (isHijack)isHijack = false;

	// カメラ刁E��替え�E力（左クリチE���E�E
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT && enableHijack)
	{
		if (useCam)
			isChange = true; // 允E��戻ぁE
		else {
			isHijack = true; // 新たにハイジャチE��開姁E
			changeCameraInSE->Play(false);
		}

		useCam = !useCam;
	}

	// ハイジャチE��時間刁E��で強制解除
	if (enableHijackTime <= 0 && useCam)
	{
		useCam = false;
		isChange = true;
	}
}


// ハイジャチE��関連の状態更新
void Player::UpdateHijack(float dt)
{
	// 攻撁E��れたら強制解除
	if (isHit)useCam = false;

	enableHijack = true;
	if (enableHijackTime < 8.0f && !useCam)
		enableHijack = false;

	// ハイジャチE��開始時のコスト消費
	if (isHijack)
	{
		enableHijackTime -= hijackCost;
	}

	// ハイジャチE��中のゲージ消費
	if (useCam)
	{
		enableHijackTime -= hijackCostPerSec * dt;
		changeCameraKeepSE->Play(true);
	}
	else // 非ハイジャチE��時�Eゲージ回復
	{
		if (changeCameraKeepSE->IsPlaying()) {
			changeCameraKeepSE->Stop();
		}
		if (maxHijackTime > enableHijackTime)
		{
			enableHijackTime += hijackRecoveryPerSec * dt;

			if (enableHijackTime > maxHijackTime)
				enableHijackTime = maxHijackTime;
		}
	}
}

// アニメーション更新
void Player::UpdateAnimation(float dt)
{
	if (!model->GetResource()->GetAnimations().empty())
		animationController.UpdateAnimation(dt);
}

// 死亡時�E演�E処琁E��カメラ回転など�E�E
void Player::DeathState(float dt)
{
	time += dt;

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

	pitch = asinf(y);       // ピッチ见E
	yaw = atan2f(x, z);     // ヨー见E

	// 敵方向への回転補間
	{
		DirectX::XMVECTOR Dot, Cross;
		DirectX::XMFLOAT3 crossVector;
		float dot;
		Dot = DirectX::XMVector3Dot(Forward, PlayerToEnemyDir);
		Cross = DirectX::XMVector3Cross(Forward, PlayerToEnemyDir);
		DirectX::XMStoreFloat(&dot, Dot);
		DirectX::XMStoreFloat3(&crossVector, Cross);
    
		float radian = acosf(dot);
		if (crossVector.y < 0 && radian > 0)
			radian *= -1;

		time = DirectX::XMMin(time, totalTime);

		static float angleX = angle.x;
		static float angleY = angle.y;

		if (angle.x < 0)
			angle.x = Easing::InSine(time, totalTime, 0.0f, angleX);
		else
			angle.x = Easing::InSine(time, totalTime, angleX, 0.0f);

		angle.y = Easing::OutBack(time, totalTime, 1.0f, angleY, radian);
	}
}