#include "Player.h"
#include "System/Input.h"
#include "Camera/Camera.h"
#include "System/Audio.h"
#include "imgui.h"
#include <algorithm>
#include "Math/Easing.h"
#include "System/CollisionEditor.h"
#include "System/SettingsManager.h"

static bool hit = false;
static float time = 0;
static constexpr float totalTime = 1;
static bool staticIsDeathStart = false;
/// コンストラクタ
Player::Player(const DirectX::XMFLOAT3& position)
{
	/// プレイヤーモデル
	model = std::make_unique<Model>("./Data/Model/Player/player_mesh.mdl");

	/// プレイヤーのパラメータ初期設定
	{
		this->position = position;				  ///< ポジション
		scale = { 0.015, 0.015, 0.015 };    ///< スケール
		viewPoint = 1.5;                    ///< カメラの視点用
		radius = 0.6;                       ///< 当たり判定用半径
		enableHijackTime = maxHijackTime;   ///< 敵の視点をハイジャックできる時間の初期化
		acceleration = 1.1f;                ///< 加速度
		deceleration = 5.0f;                ///< 減速度
		hit = false;                        ///< 死亡演出用(グローバル変数)
		time = 0;                           ///< 死亡演出用(グローバル変数) 
		staticIsDeathStart = false;			///< 死亡演出用(グローバル変数	)
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

	if (Difficulty::Instance().GetDifficulty() == 2)
	{
		hijackRecoveryPerSec = 1;
	}
	else
	{
		hijackRecoveryPerSec = 3;
	}
}

/// デストラクタ
Player::~Player()
{
}

/// 更新処理
void Player::Update(float dt)
{
	/// ハイジャックの時間処理
	UpdateHijack(dt);

	/// カメラ切り替え処理(実際のカメラの切り替えはSceneでやってる)
	/// カメラを切り替えたときの処理、フラグを更新してる
	 
	GameSettings setting = SettingsManager::Instance().GetGameSettings();
	changeCameraInSE->SetVolume(0.5f * setting.seVolume * setting.masterVolume);
	changeCameraKeepSE->SetVolume(1.0f * setting.seVolume * setting.masterVolume);

	ChangeCamera();

	// �v���C���[�ړ�����

	if (isEvent) ///< Move() の中でフラグの切り替えをしてる
		DeathState(dt);
	else
		Move(dt);

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

		//float radian = CalcAngle();
		//std::string text2;
		//if (radian < 0.78 && radian > -0.78)text2 = "front";
		//else if (radian < 2.3 && radian >= 0.78)text2 = "right";
		//else text2 = "???";
		//ImGui::Text(text2.c_str());
		//ImGui::SameLine();
		//ImGui::InputFloat("radian", &radian);
		//DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&world);
		//DirectX::XMVECTOR Forward = DirectX::XMVector3Normalize(M.r[2]);
		//DirectX::XMFLOAT3 forward; DirectX::XMStoreFloat3(&forward, Forward);
		//ImGui::InputFloat3("forward", &forward.x);
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
		acceleration = 0;
		hit = isHit;
	}

	/// 敵と接触した場合はだんだん速度を落として演出に入る
	if ((CalcAngle() < 0.78 && CalcAngle() > -0.78) && hit)
	{
		isEvent = true;
		deathType = 0;
		if (!deathStart)
		{
			speed = -4.5;
			deathStart = !deathStart;
		}

		if (speed < 0)
			accel += 1.5f;
		else
		{
			speed = 0;
			accel = 0;
		}
	}
	else if (hit)
	{
		deathType = 1;
		if (speed > 0)
			accel -= deceleration * dt;
		else
			isEvent = true;
	}
	else /// 死んでない時
	{
		/// 加速処理
		accel += acceleration * dt;
	}

	Camera& cam = Camera::Instance();

	DirectX::XMFLOAT3 forward;
	// カメラが切り替わっていないときだけカメラの方向を取る
	if (!useCam && !hit)
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
	//speed = DirectX::XMMax(speed, 0.0f);

	//position.x += speed * forward.x * dt;
	//position.z += speed * forward.z * dt;

	// ---------- 壁との当たり判定と押し出し処理 ----------
	DirectX::XMFLOAT3 outPos = {};
	DirectX::XMFLOAT3 pushDir = {};

	float adjustedSpeed = speed;

	if (CollisionEditor::Instance().Collision(position, radius, outPos, pushDir))
	{
		DirectX::XMVECTOR moveDirVec = DirectX::XMLoadFloat3(&forward);
		DirectX::XMVECTOR pushDirVec = DirectX::XMLoadFloat3(&pushDir);

		float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(moveDirVec, pushDirVec));

		if (dot < 0.0f) // 移動方向と押し出し方向が逆なら押し出す
		{
			float slowFactor = 1.0f - fabsf(dot); // 壁との角度に応じて減速
			adjustedSpeed *= slowFactor;


			DirectX::XMVECTOR currentPos = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR targetPos = DirectX::XMLoadFloat3(&outPos);

			// 速度に応じて補間係数を調整（滑らかに）
			float lerpFactor = DirectX::XMMin(0.8f, speed / maxSpeed);
			DirectX::XMVECTOR smoothedPos = DirectX::XMVectorLerp(currentPos, targetPos, lerpFactor);

			DirectX::XMStoreFloat3(&position, smoothedPos);
		}
	}

	if (!inGate) ///< ゲートに入ったらプレイヤーは移動しない
	{
		//position.x += speed * forward.x * dt;
		//position.z += speed * forward.z * dt;

		// 減衰後の速度で移動
		position.x += adjustedSpeed * forward.x * dt;
		position.z += adjustedSpeed * forward.z * dt;
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
	if (hit)return;
	if (enableOpenGate && !useCam) return; ///< ドアが開ける場合

	Mouse& mouse = Input::Instance().GetMouse();

	if (isChange)
		isChange = false;
	if (isHijack)isHijack = false;

	// �E�N���b�N�Ő؂�ւ�
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT && enableHijack)
	{
		if (useCam)
			isChange = true; // �؂�ւ�����
		else {
			isHijack = true; // �n�C�W���b�N�J�n
			changeCameraInSE->Play(false);
		}

		useCam = !useCam;
		hijackedElapsedTime = 0;
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
		hijackedElapsedTime += dt;
		if (hijackedElapsedTime> hijack_risc_time)
		{
			enemyRef.get()->remote_sensing(position);
		}
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

	static float staticViewPoint = viewPoint;
	if (deathType == 0) ///< 前
	{
		viewPoint = Easing::OutBack(time, totalTime, 0.8f, 0.3f, staticViewPoint);
		if (viewPoint >= 0.4 && pitch < 60* 0.01745f)
			pitch += 50 * 0.01745f * dt;

		if (speed < 0)
			accel += 1.5f;
		else
		{
			speed = 0;
			accel = 0;
		}

		speed += accel * dt;
		position.x += speed * saveDirection.x * dt;
		position.z += speed * saveDirection.z * dt;
	}
	else if (deathType == 1) ///< 後ろ
	{
		viewPoint = Easing::OutBack(time, totalTime, 0.8f, 0.1f, staticViewPoint);
	}

	
	static float radian;
	if (!staticIsDeathStart)
	{
		radian = CalcAngle();
		angle.x = 0;
	}

	time = DirectX::XMMin(time, totalTime);
	static float angleX = angle.x;
	static float angleY = angle.y;

	angle.y = Easing::OutBack(time, totalTime, 0.8f, radian + angleY, angleY);
	//angle.x = Easing::OutBack(time, totalTime, 0.8f, 0.0f, angleX);
	staticIsDeathStart = true;
}

/// 角度計算用
float Player::CalcAngle()
{
	using namespace DirectX;
	XMFLOAT3 enemyPos;
	if (enemyRef)
		enemyPos = enemyRef->GetPosition();

	XMVECTOR PlayerToEnemy = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&enemyPos), XMLoadFloat3(&position))); ///< プレイヤーから敵のベクトル
	XMMATRIX M = XMLoadFloat4x4(&world);
	XMVECTOR Forward = M.r[2];

	/// プレイヤーのセーブピッチヨーを計算
	{
		float x, y, z;
		x = DirectX::XMVectorGetX(Forward);
		z = DirectX::XMVectorGetZ(Forward);
		y = DirectX::XMVectorGetY(Forward);

		pitch = asinf(y);        // 上下の向き
		yaw = atan2f(x, z);      // 左右の向き
	}

	XMVECTOR Dot = XMVector3Dot(DirectX::XMVector3Normalize(Forward), PlayerToEnemy);
	float dot;
	dot = std::clamp(dot, -1.0f, 1.0f);
	XMStoreFloat(&dot ,Dot);
	float radian = acosf(dot);

	XMVECTOR Cross = XMVector3Cross(Forward, PlayerToEnemy);
	XMFLOAT3 crossVec;
	XMStoreFloat3(&crossVec, Cross);

	if (crossVec.y < 0)
		radian *= -1;

	return radian;
}
