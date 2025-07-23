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
	// �e�X�g�p���f���̓ǂݍ���
	model = std::make_shared<Model>("./Data/Model/Test/test_walk_animation_model.mdl");
	t_position.x += 0.2f;
	t_position.z += 0.5f;
	t_position.y = 1.15f;
	t_scale = { 0.025,0.025,0.025 };
#else
	// ���i�Ńv���C���[���f���̓ǂݍ���
	model = std::make_unique<Model>("./Data/Model/Player/player_mesh.mdl");
#endif

	// �����p�����[�^�̐ݒ�
	{
		position = { 1,0,-24 };
		scale = { 0.015, 0.015, 0.015 };
		viewPoint = 1.5;           // ���_�̍���
		radius = 0.6;              // �����蔻�蔼�a
		enableHijackTime = maxHijackTime;   // �n�C�W���b�N�\���Ԃ̏����l
		acceleration = 1.1f;
		deceleration = 1.2f;
		hit = false;
		time = 0;
	}

	// �A�j���[�V���������ݒ�
	{
		animationController.SetModel(model);
		animationController.PlayAnimation(static_cast<int>(AnimationState::MOVE), true);
		animationController.SetAnimationSecondScale(1.0f);
	}

	// �}�e���A���e�N�X�`���ǂݍ���
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
	// �n�C�W���b�N�֘A�̍X�V����
	UpdateHijack(dt);

	// �J�����؂�ւ������i���������j
	//ChangeCamera();

	// �v���C���[�ړ�����
	Move(dt);

	if (isEvent) // �ړ����ɃC�x���g��Ԃֈڍs
		DeathState(dt);

#ifdef TEST
	TestTransformUpdate();
#endif

	// ���f�����[���h�s��̍X�V
	UpdateTransform();

	// �A�j���[�V�����̍X�V
	UpdateAnimation(dt);

	// ���f���Ƀ��[���h�s���K�p
	model->UpdateTransform();
}

// ���f���̕`��
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
#ifndef TEST
	// �e�N�X�`���ݒ�
	textures->Set(rc);

	// ���f�������݂��A�J�����؂�ւ����ł���Ε`��
	if (model && useCam)
		renderer->Render(rc, world, model.get(), ShaderId::Custom);

	// �e�N�X�`���̉���
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

// �f�o�b�O�`��iImGui�j
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

// �v���C���[�ړ�����
void Player::Move(float dt)
{
	if (!hit && isHit) // ���߂čU�����󂯂����̏���
	{
		accel = 0;
		hit = isHit;
	}

	// ��������
	if (hit)
	{
		if (speed > 0)
			accel -= deceleration * dt;
		else
			isEvent = true;
	}
	else
	{
		// ��������
		accel += acceleration * dt;
	}

	Camera& cam = Camera::Instance();

	DirectX::XMFLOAT3 forward;
	// �ʏ�J���� or �n�C�W���b�N���_
	if (!useCam)
	{
		forward = cam.GetFront();
	}
	else
		forward = saveDirection;

	// XZ ���ʂ̕����x�N�g���ɐ��K��
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

	// �v���C���[�̌����v�Z�iY����]�j
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

// �J�����̐؂�ւ������i���N���b�N�j
void Player::ChangeCamera()
{
	if (isHit)return;

	Mouse& mouse = Input::Instance().GetMouse();

	if (isChange)isChange = false;
	if (isHijack)isHijack = false;

	// �J�����؂�ւ����́i���N���b�N�j
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT && enableHijack)
	{
		if (useCam)
			isChange = true; // ���ɖ߂�
		else
			isHijack = true; // �V���Ƀn�C�W���b�N�J�n

		useCam = !useCam;
	}

	// �n�C�W���b�N���Ԑ؂�ŋ�������
	if (enableHijackTime <= 0 && useCam)
	{
		useCam = false;
		isChange = true;
	}
}

// �n�C�W���b�N�֘A�̏�ԍX�V
void Player::UpdateHijack(float dt)
{
	// �U�����ꂽ�狭������
	if (isHit)useCam = false;

	enableHijack = true;
	if (enableHijackTime < 8.0f && !useCam)
		enableHijack = false;

	// �n�C�W���b�N�J�n���̃R�X�g����
	if (isHijack)
	{
		enableHijackTime -= hijackCost;
	}

	// �n�C�W���b�N���̃Q�[�W����
	if (useCam)
	{
		enableHijackTime -= hijackCostPerSec * dt;
	}
	else // ��n�C�W���b�N���̃Q�[�W��
	{
		if (maxHijackTime > enableHijackTime)
		{
			enableHijackTime += hijackRecoveryPerSec * dt;

			if (enableHijackTime > maxHijackTime)
				enableHijackTime = maxHijackTime;
		}
	}
}

// �A�j���[�V�����X�V
void Player::UpdateAnimation(float dt)
{
	if (!model->GetResource()->GetAnimations().empty())
		animationController.UpdateAnimation(dt);
}

// ���S���̉��o�����i�J������]�Ȃǁj
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

	pitch = asinf(y);       // �s�b�`�p
	yaw = atan2f(x, z);     // ���[�p

	// �G�����ւ̉�]���
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
