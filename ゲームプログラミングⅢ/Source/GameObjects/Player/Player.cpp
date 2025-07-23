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
	// 繝・せ繝育畑繝｢繝・Ν縺ｮ隱ｭ縺ｿ霎ｼ縺ｿ
	model = std::make_shared<Model>("./Data/Model/Test/test_walk_animation_model.mdl");
	t_position.x += 0.2f;
	t_position.z += 0.5f;
	t_position.y = 1.15f;
	t_scale = { 0.025,0.025,0.025 };
#else
	// 陬ｽ蜩∫沿繝励Ξ繧､繝､繝ｼ繝｢繝・Ν縺ｮ隱ｭ縺ｿ霎ｼ縺ｿ
	model = std::make_unique<Model>("./Data/Model/Player/player_mesh.mdl");
#endif

	// 蛻晄悄繝代Λ繝｡繝ｼ繧ｿ縺ｮ險ｭ螳・
	{
		position = { 1,0,-24 };
		scale = { 0.015, 0.015, 0.015 };
		viewPoint = 1.5;           // 隕也せ縺ｮ鬮倥＆
		radius = 0.6;              // 蠖薙◆繧雁愛螳壼濠蠕・
		enableHijackTime = maxHijackTime;   // 繝上う繧ｸ繝｣繝・け蜿ｯ閭ｽ譎る俣縺ｮ蛻晄悄蛟､
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

    // SEの読み込み
    changeCameraInSE = Audio::Instance().LoadAudioSource("Data/Sound/change_camera_in.wav");
    changeCameraKeepSE = Audio::Instance().LoadAudioSource("Data/Sound/change_camera_keep.wav");
}

Player::~Player()
{

}

void Player::Update(float dt)
{
	// 繝上う繧ｸ繝｣繝・け髢｢騾｣縺ｮ譖ｴ譁ｰ蜃ｦ逅・
	UpdateHijack(dt);

    // 繧ｫ繝｡繝ｩ蛻・ｊ譖ｿ縺亥・逅・
    if (changeCameraInSE->IsPlaying())
        changeCameraInSE->SetVolume(0.5f);

	// カメラ切り替え処理
	ChangeCamera();

	Move(dt);

	if (isEvent) // 遘ｻ蜍穂ｸｭ縺ｫ繧､繝吶Φ繝育憾諷九∈遘ｻ陦・
		DeathState(dt);

#ifdef TEST
	TestTransformUpdate();
#endif
	// 繝｢繝・Ν繝ｯ繝ｼ繝ｫ繝芽｡悟・縺ｮ譖ｴ譁ｰ
	UpdateTransform();

	// 繧｢繝九Γ繝ｼ繧ｷ繝ｧ繝ｳ縺ｮ譖ｴ譁ｰ
	UpdateAnimation(dt);

	// 繝｢繝・Ν縺ｫ繝ｯ繝ｼ繝ｫ繝芽｡悟・繧帝←逕ｨ
	model->UpdateTransform();
}

// 繝｢繝・Ν縺ｮ謠冗判
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
#ifndef TEST
	// 繝・け繧ｹ繝√Ε險ｭ螳・
	textures->Set(rc);

	// 繝｢繝・Ν縺悟ｭ伜惠縺励√き繝｡繝ｩ蛻・ｊ譖ｿ縺井ｸｭ縺ｧ縺ゅｌ縺ｰ謠冗判
	if (model && useCam)
		renderer->Render(rc, world, model.get(), ShaderId::Custom);

	// 繝・け繧ｹ繝√Ε縺ｮ隗｣髯､
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

// 繝・ヰ繝・げ謠冗判・・mGui・・
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

// プレイヤー移動処理
void Player::Move(float dt)
{
	if (!hit && isHit) // 蛻昴ａ縺ｦ謾ｻ謦・ｒ蜿励￠縺滓凾縺ｮ蜃ｦ逅・
	{
		accel = 0;
		hit = isHit;
	}

	// 貂幃溷・逅・
	if (hit)
	{
		if (speed > 0)
			accel -= deceleration * dt;
		else
			isEvent = true;
	}
	else
	{
		// 蜉騾溷・逅・
		accel += acceleration * dt;
	}

	Camera& cam = Camera::Instance();

	DirectX::XMFLOAT3 forward;
	// 騾壼ｸｸ繧ｫ繝｡繝ｩ or 繝上う繧ｸ繝｣繝・け隕也せ
	if (!useCam)
	{
		forward = cam.GetFront();
	}
	else
		forward = saveDirection;

	// XZ 蟷ｳ髱｢縺ｮ譁ｹ蜷代・繧ｯ繝医Ν縺ｫ豁｣隕丞喧
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

	// 繝励Ξ繧､繝､繝ｼ縺ｮ蜷代″險育ｮ暦ｼ・霆ｸ蝗櫁ｻ｢・・
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

// 繧ｫ繝｡繝ｩ縺ｮ蛻・ｊ譖ｿ縺亥・逅・ｼ亥ｷｦ繧ｯ繝ｪ繝・け・・
void Player::ChangeCamera()
{
	if (isHit)return;

	Mouse& mouse = Input::Instance().GetMouse();

	if (isChange)isChange = false;
	if (isHijack)isHijack = false;

	// 繧ｫ繝｡繝ｩ蛻・ｊ譖ｿ縺亥・蜉幢ｼ亥ｷｦ繧ｯ繝ｪ繝・け・・
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT && enableHijack)
	{
		if (useCam)
			isChange = true; // 蜈・↓謌ｻ縺・
		else {
			isHijack = true; // 譁ｰ縺溘↓繝上う繧ｸ繝｣繝・け髢句ｧ・
			changeCameraInSE->Play(false);
		}

		useCam = !useCam;
	}

	// 繝上う繧ｸ繝｣繝・け譎る俣蛻・ｌ縺ｧ蠑ｷ蛻ｶ隗｣髯､
	if (enableHijackTime <= 0 && useCam)
	{
		useCam = false;
		isChange = true;
	}
}


// 繝上う繧ｸ繝｣繝・け髢｢騾｣縺ｮ迥ｶ諷区峩譁ｰ
void Player::UpdateHijack(float dt)
{
	// 謾ｻ謦・＆繧後◆繧牙ｼｷ蛻ｶ隗｣髯､
	if (isHit)useCam = false;

	enableHijack = true;
	if (enableHijackTime < 8.0f && !useCam)
		enableHijack = false;

	// 繝上う繧ｸ繝｣繝・け髢句ｧ区凾縺ｮ繧ｳ繧ｹ繝域ｶ郁ｲｻ
	if (isHijack)
	{
		enableHijackTime -= hijackCost;
	}

	// 繝上う繧ｸ繝｣繝・け荳ｭ縺ｮ繧ｲ繝ｼ繧ｸ豸郁ｲｻ
	if (useCam)
	{
		enableHijackTime -= hijackCostPerSec * dt;
		changeCameraKeepSE->Play(true);
	}
	else // 髱槭ワ繧､繧ｸ繝｣繝・け譎ゅ・繧ｲ繝ｼ繧ｸ蝗槫ｾｩ
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

// 繧｢繝九Γ繝ｼ繧ｷ繝ｧ繝ｳ譖ｴ譁ｰ
void Player::UpdateAnimation(float dt)
{
	if (!model->GetResource()->GetAnimations().empty())
		animationController.UpdateAnimation(dt);
}

// 豁ｻ莠｡譎ゅ・貍泌・蜃ｦ逅・ｼ医き繝｡繝ｩ蝗櫁ｻ｢縺ｪ縺ｩ・・
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

	pitch = asinf(y);       // 繝斐ャ繝∬ｧ・
	yaw = atan2f(x, z);     // 繝ｨ繝ｼ隗・

	// 謨ｵ譁ｹ蜷代∈縺ｮ蝗櫁ｻ｢陬憺俣
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