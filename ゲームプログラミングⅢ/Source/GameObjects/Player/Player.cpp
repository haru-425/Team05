#include "Player.h"
#include "System/Input.h"
#include "Camera/Camera.h"
#include "System/Audio.h"
#include "imgui.h"
#include <algorithm>
#include "Math/Easing.h"
#include "System/CollisionEditor.h"
#include "System/SettingsManager.h"
#include "Camera/CameraController/FPCameraController.h"

static bool hit = false;
static float time = 0;
static constexpr float totalTime = 1;
static bool staticIsDeathStart = false;
/// コンストラクタ
Player::Player(const DirectX::XMFLOAT3& position)
{
	/// プレイヤーモデル
	model = std::make_unique<Model>("./Data/Model/Player/player_mesh2.mdl");

	/// プレイヤーのパラメータ初期設定
	{
		this->position = position;				  ///< ポジション
		scale = { 0.015, 0.015, 0.015 };    ///< スケール
		//viewPoint = 1.5;                    ///< カメラの視点用
		radius = 0.6;                       ///< 当たり判定用半径
		enableHijackTime = maxHijackTime;   ///< 敵の視点をハイジャックできる時間の初期化
		acceleration = 1.1f;                ///< 加速度
		deceleration = 5.0f;                ///< 減速度
		hit = false;                        ///< 死亡演出用(グローバル変数)
		time = 0;                           ///< 死亡演出用(グローバル変数) 
		staticIsDeathStart = false;			///< 死亡演出用(グローバル変数	)

		firstEntry = false;
		secondEntry = false;
		eventTimer = 0;
		eventStart = false;
		isDeath = false;
	}

	/// アニメーション関係の設定(今回はアニメーションはなし)
	{
	    animationController.SetModel(model);
		animationController.PlayAnimation(static_cast<int>(AnimationState::POSE), false);
	    animationController.SetAnimationSecondScale(1.0f);
	}

	/// テクスチャの読み込み
	textures = std::make_unique<LoadTextures>();
	textures->LoadNormal("Data/Model/Player/Texture/player_mtl_Normal_DirectX.png");
	textures->LoadMetalness("Data/Model/Player/Texture/player_mtl_Metallic.png");
	textures->LoadEmisive("Data/Model/Player/Texture/player_mtl_Emissive.png");
	textures->LoadOcclusion("Data/Model/Player/Texture/player_mtl_Opacity.png");

	/// SEの読み込み
	changeCameraInSE     = Audio::Instance().LoadAudioSource("Data/Sound/change_camera_in.wav");
	changeCameraKeepSE   = Audio::Instance().LoadAudioSource("Data/Sound/change_camera_keep.wav");
	changeCameraFailedSE = Audio::Instance().LoadAudioSource("Data/Sound/change_camera_failed.wav");

	if (Difficulty::Instance().GetDifficulty() == 2)
	{
		hijackRecoveryPerSec = 1;
	}
	else
	{
		hijackRecoveryPerSec = 3;
	}

	for (Model::Node& node : model->GetNodes())
	{
		if (strcmp("eye_point", node.name) == 0)
		{
			viewPoint = node.translate.y * 0.015f;
		}
	}
}

/// デストラクタ
Player::~Player()
{
}

void Player::UpdateSounds(float dt)
{
	GameSettings setting = SettingsManager::Instance().GetGameSettings();
	changeCameraInSE->SetVolume(0.5f * setting.seVolume * setting.masterVolume);
	changeCameraKeepSE->SetVolume(1.0f * setting.seVolume * setting.masterVolume);
	changeCameraFailedSE->SetVolume(1.0f * setting.seVolume * setting.masterVolume);
}

bool isDiscovered = false;
bool look = false;
/// 更新処理
void Player::Update(float dt)
{
	/// ハイジャックの時間処理
	/// カメラ切り替え処理(実際のカメラの切り替えはSceneでやってる)
	/// カメラを切り替えたときの処理、フラグを更新してる
	UpdateHijack(dt);

	/// 目線の位置
	for (Model::Node& node : model->GetNodes())
	{
		if (strcmp("eye_point", node.name) == 0)
		{
			viewPoint = node.translate.y * 0.015f;
		}
	}

	/// 敵に追跡されているのか(コントローラーの振動)
	{
		if (!isEvent)
		{
			if (enemyRef->Get_Tracking() || enemyRef->Get_isPlayerInView())
			{
				look = true;
			}
			else
			{
				look = false;
			}
		}
		else
		{
			Input::Instance().GetGamePad().strength.x = 0;
			Input::Instance().GetGamePad().strength.y = 0;
			look = false;
		}

		if (look)
		{
			vibrationTimer += dt;
			float bpm = 60.0f;
			float freq = bpm / 60.0f;
			float wave = (sinf(vibrationTimer * freq * 2.0f * 3.14f) + 1.0f) * 0.5f;
			float vibration = powf(wave, 3.0f);
			Input::Instance().GetGamePad().strength.x = vibration/*std::sinf(0.125f * vibrationTimer) * 0.5f + 0.25f*/;
			Input::Instance().GetGamePad().strength.y = vibration/*std::sinf(0.125f * vibrationTimer) * 0.5f + 0.25f*/;
		}
		else
		{
			Input::Instance().GetGamePad().strength = { 0,0 };
		}
	}

	if(!inGate)
		Dash(dt);

	ChangeCamera();

	if (isEvent) ///< Move() の中でフラグの切り替えをしてる
		DeathState(dt);
	else
	{
		Move(dt);
		pitch = angle.x;
		yaw = angle.y;
	}

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
		float animationLength = animationController.GetAnimationSeconds(static_cast<UINT>(0));
		ImGui::InputFloat("animationLength", &animationLength);
		ImGui::InputFloat("pitch", &pitch);
		ImGui::InputFloat("yaw", &yaw);
		ImGui::InputFloat3("angle", &angle.x);
    
		ImGui::Checkbox("enableDash", &enableDash);
		ImGui::Checkbox("isDash", &isDash);
		ImGui::Checkbox("look", &look);
		ImGui::InputFloat("enableDash", &dashTimer);
		ImGui::InputFloat("speed", &speed);

		bool isRotating = FPCameraController::GetIsRotating();
		ImGui::Checkbox("isRotating", &isRotating);
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
	if (changeCameraFailedSE) {
		changeCameraFailedSE->Stop();
		delete changeCameraFailedSE;
		changeCameraFailedSE = nullptr;
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

		// イベント開始
		isEvent = true;

		accel = 0;
		speed -= 1 * dt;
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

	/// スピード
	speed = DirectX::XMMin(speed, maxSpeed + dashSpeed);;

	// ---------- 壁との当たり判定と押し出し処理 ----------
	DirectX::XMFLOAT3 outPos = {};
	DirectX::XMFLOAT3 pushDir = {};

	//float adjustedSpeed = speed;

	if (CollisionEditor::Instance().Collision(position, radius, outPos, pushDir))
	{
		DirectX::XMVECTOR moveDirVec = DirectX::XMLoadFloat3(&forward);
		DirectX::XMVECTOR pushDirVec = DirectX::XMLoadFloat3(&pushDir);

		float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(moveDirVec, pushDirVec));

		if (dot < 0.0f) // 移動方向と押し出し方向が逆なら押し出す
		{
			//float slowFactor = 1.0f - fabsf(dot); // 壁との角度に応じて減速
			//adjustedSpeed *= slowFactor;

			DirectX::XMVECTOR currentPos = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR targetPos = DirectX::XMLoadFloat3(&outPos);

			// 速度に応じて補間係数を調整（滑らかに）
			float lerpFactor = DirectX::XMMin(0.8f, speed / maxSpeed);
			DirectX::XMVECTOR smoothedPos = DirectX::XMVectorLerp(currentPos, targetPos, lerpFactor);

			DirectX::XMStoreFloat3(&position, smoothedPos);
		}
	}

	bool isRotating = FPCameraController::GetIsRotating();

	float slow = 1;

	if (!inGate) ///< ゲートに入ったらプレイヤーは移動しない
	{
		if (!isRotating) {
			position.x += speed * forward.x * dt;
			position.z += speed * forward.z * dt;
		}
		else {
			/// 旋回中は減速
			position.x += speed * slow * forward.x * dt;
			position.z += speed * slow * forward.z * dt;
		}
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
	GamePad& gamePad = Input::Instance().GetGamePad();

	if (isChange)
		isChange = false;
	if (isHijack)isHijack = false;

	// �E�N���b�N�Ő؂�ւ�
	if ((mouse.GetButtonDown() & Mouse::BTN_LEFT || gamePad.GetButtonDown() & GamePad::BTN_RIGHT_SHOULDER) && enableHijack)
	{
		if (enableHijack) {
			if (useCam)
				isChange = true; // �؂�ւ�����
			else {
				isHijack = true; // �n�C�W���b�N�J�n
				changeCameraInSE->Play(false);
			}

			useCam = !useCam;
			hijackedElapsedTime = 0;
		}
		else
		{
			if (changeCameraFailedSE->IsPlaying()) {
				changeCameraFailedSE->Stop();
			}
			changeCameraFailedSE->Play(false);
		}
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
	time = std::clamp(time, 0.0f, 1.0f);
	eventTimer = Easing::InSine(time, animationController.GetAnimationSeconds(0) * 0.5f);

	static float radian;
	static float lastAngle = 0; // 演出！！！
	static float eventYaw;

	const float animationSecondScale = 1.5f;
	if (deathType == 0) ///< 前
	{
		eventStart = true;
		static float eventPitch = 0;
		if (!firstEntry)
		{
			animationController.PlayAnimation("eye_point|back_deth_animation", false);
			animationController.SetAnimationSecondScale(animationSecondScale);
			firstEntry = true;
			speed = -maxSpeed;

			eventPitch = pitch;
		}
		
		pitch = Easing::OutExp(eventTimer, animationController.GetAnimationSeconds(0) * 0.5f, -30.0f * 0.01745f, eventPitch);

		angleTimer += dt;
		angleTimer = std::clamp(angleTimer, 0.0f, 3.0f);
		yaw = Easing::InQuint(angleTimer, 1.0f, radian + eventYaw, yaw);
		if (!secondEntry)
		{
			radian = CalcAngle();
			eventYaw = yaw; // イベント発生時のよー角
			secondEntry = true;
		}
		if (speed < 0)
			accel += 2.0f;
		else
		{
			speed = 0;
			accel = 0;
		}

		speed += accel * dt;
		position.x += speed * saveDirection.x * dt;
		position.z += speed * saveDirection.z * dt;

		if (angleTimer >= 3.0f)
			isDeath = true;
	}
	else if (deathType == 1) ///< 後ろ
	{
		if (!firstEntry)
		{
			animationController.PlayAnimation("eye_point|back_deth_animation", false);
			animationController.SetAnimationSecondScale(animationSecondScale);
			firstEntry = true;
		}

		if (animationController.GetEndAnimation())
		{
			if (!secondEntry)
			{
				radian		= CalcAngle();
				eventYaw	= yaw; // イベント発生時のよー角
				secondEntry = true;
			}

			angleTimer += dt;
			angleTimer = std::clamp(angleTimer, 0.0f, 3.0f);
			yaw = Easing::InQuint(angleTimer, 2.0f, radian + eventYaw, yaw);
			pitch = Easing::OutBack(angleTimer, 1.8f, 0.5f, -40.0f * 0.01745f, lastAngle);
			if (angleTimer > 1.5f)
				eventStart = true;
		}
		else
		{
			pitch = Easing::OutExp(eventTimer, animationController.GetAnimationSeconds(0) * 0.5f, 50 * 0.01745f, -70 * 0.01745f);
			lastAngle = yaw;
		}

		if (speed > 0)
			accel -= 1.5f;
		else
		{
			speed = 0;
			accel = 0;
		}

		speed += accel * dt;
		position.x += speed * saveDirection.x * dt;
		position.z += speed * saveDirection.z * dt;

		if (angleTimer >= 2.0)
			isDeath = true;
	}

	if (isDeath)
		Input::Instance().GetGamePad().strength = { 1,1 };

	staticIsDeathStart = true;
}

/// 角度計算用
float Player::CalcAngle()
{
	using namespace DirectX;
	XMFLOAT3 enemyPos = {};
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

		angle.x = asinf(y);        // 上下の向き
		angle.y = atan2f(x, z);      // 左右の向き
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

/**
* ダッシュ関数
*/
void Player::Dash(float elapsedTime)
{
	auto& gamePad = Input::Instance().GetGamePad();
	
	/// 走っているか
	if (isDash)
	{
		if (dashTimer > 2.2f)
			dashSpeed += 5 * elapsedTime;
		else if (dashTimer < 0.5f)
			dashSpeed -= 6 * elapsedTime;

		dashTimer -= elapsedTime;

		dashSpeed = std::clamp(dashSpeed, 0.0f, maxDashSpeed);
	}

	/// ダッシュ時間が切れたかどうか
	if (dashTimer < 0)
	{
		isDash = false;
		dashTimer = 3;
	}

	/// ダッシュ状態じゃない場合にタイマーを減らす
	if (!isDash)
	{
		dashAvailableTimer += elapsedTime;
	}
	dashAvailableTimer = std::clamp(dashAvailableTimer, 0.0f, dashCoolTime);

	/// ダッシュが可能になるまでのタイマーが40になると走れるようになる
	if (dashAvailableTimer == 40)
	{
		enableDash = true;
	}

	/// ダッシュできる状態かつボタンを押したとき
	if ((gamePad.GetButtonDown() & GamePad::SHIFT/*PC*/ || gamePad.GetButtonDown() & GamePad::BTN_LEFT_SHOULDER/*コントローラー*/) && enableDash)
	{
		isDash = true;
		enableDash = false;
		dashAvailableTimer = 0;
	}
}
