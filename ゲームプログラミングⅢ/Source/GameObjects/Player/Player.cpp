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
/// コンストラクタ
Player::Player(const DirectX::XMFLOAT3& position)
{
#ifdef TEST
	// �e�X�g�p�A�j���[�V�������f���̓ǂݍ���// プレイヤーモデル
	model = std::make_shared<Model>("./Data/Model/Test/test_walk_animation_model.mdl");
	t_position.x += 0.2f;
	t_position.z += 0.5f;
	t_position.y = 1.15f;
	t_scale = { 0.025,0.025,0.025 };
#else
	// �{�ԗp�v���C���[���f���̓ǂݍ���
	model = std::make_unique<Model>("./Data/Model/Player/player_mesh.mdl");

	/// プレイヤーのパラメータ初期設定
	{
		position = { 1,0,-24 };
		scale = { 0.015, 0.015, 0.015 };
		viewPoint = 1.5;           // ���_�̍���// カメラの視点用
		radius = 0.6;              // �v���C���[�̔��a�i�����蔻��p�j//当たり判定用半径
		enableHijackTime = maxHijackTime;   // �n�C�W���b�N�\�ȍő厞��//敵の視点をハイジャックできる時間の初期化
		acceleration = 1.1f;
		deceleration = 1.2f;
		hit = false;
		time = 0;
	}

  /// アニメーション関係の設定(今回はアニメーションはなし)
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


  /// SEの読み込み
  changeCameraInSE = Audio::Instance().LoadAudioSource("Data/Sound/change_camera_in.wav");
  changeCameraKeepSE = Audio::Instance().LoadAudioSource("Data/Sound/change_camera_keep.wav");
}

/// デストラクタ
Player::~Player()
{
}

/// 更新処理
void Player::Update(float dt)
{
	// �n�C�W���b�N�֘A�̍X�V����
	UpdateHijack(dt);

	// �J�����p��SE�̉��ʒ���
	if (changeCameraInSE->IsPlaying())
		changeCameraInSE->SetVolume(0.5f);

	/// カメラ切り替え処理(実際のカメラの切り替えはSceneでやってる)
  /// カメラを切り替えたときの処理、フラグを更新してる
	ChangeCamera();

	// �v���C���[�ړ�����
	Move(dt);

	if (isEvent) ///< Move() の中でフラグの切り替えをしてる
		DeathState(dt);

	/// 行列更新処理
	UpdateTransform();

	/// アニメーション更新処理(今回はアニメーションなし)
	UpdateAnimation(dt);

	/// モデルの行列更新処理
	model->UpdateTransform();
}

/// 描画処理
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
  /// テクスチャのセット
	textures->Set(rc);

    /// モデルがあるときかつ、プレイヤーが敵カメラを使っている場合
	/// プレイヤーを描画するとどうしても、モデルとカメラが被ってしまうので、
	/// 敵視点の時のみの描画にする
	if (model && useCam)
		renderer->Render(rc, world, model.get(), ShaderId::Custom);

	/// テクスチャのクリア
	textures->Clear(rc);
}

/// デバッグ描画処理
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

		ImGui::Checkbox("enableOpenDoor", &enableOpenGate);
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

/// プレイヤー移動処理
void Player::Move(float dt)
{
	if (!hit && isHit)///< やりかたは汚いけど、一度ヒットしたらそれ以降はヒット判定にするために書く hit はPlayerコンストラクタの上でグローバルとしておいてる
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

	speed += accel * dt;
	speed = DirectX::XMMin(speed, maxSpeed);
	speed = DirectX::XMMax(speed, 0.0f);

	if (!inGate) ///< ゲートに入ったらプレイヤーは移動しない
	{
		position.x += speed * forward.x * dt;
		position.z += speed * forward.z * dt;
	}

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
	if (enableOpenGate && !useCam) return; ///< ドアが開ける場合

	Mouse& mouse = Input::Instance().GetMouse();

	if (isChange)isChange = false;
	if (isHijack)isHijack = false;

	// �E�N���b�N�Ő؂�ւ�
	if (mouse.GetButtonDown() & Mouse::BTN_RIGHT && enableHijack)
	{
		if (useCam)
			isChange = true; // �؂�ւ�����
		else {
			isHijack = true; // �n�C�W���b�N�J�n
			changeCameraInSE->Play(false);
		}

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
		enableHijackTime -= hijackCostPerSec * dt;
		changeCameraKeepSE->Play(true);
	}
	else // 視界がプレイヤーの場合
	{
		if (changeCameraKeepSE->IsPlaying()) {
			changeCameraKeepSE->Stop();
		}
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

// 死亡演出用  
void Player::DeathState(float dt)
{
	time += dt; ///< 演出用に使うタイマー(グローバル変数)

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

	pitch = asinf(y);        // 上下の向き
	yaw = atan2f(x, z);      // 左右の向き

	  // 角度を求める
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
