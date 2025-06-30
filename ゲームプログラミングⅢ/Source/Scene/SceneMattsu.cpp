#include "SceneMattsu.h"
#include "System/Graphics.h"
#include "Camera/Camera.h"
#include "System/GamePad.h"
#include "System/Input.h"
#include "Collision.h"

void SceneMattsu::Initialize()
{
	//�X�e�[�W������
	stage = new Stage();

	//�J���������ݒ�
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);

	//�J�����R���g���[���[������
	i_CameraController = std::make_unique<FPCameraController>();

	player = std::make_shared<Player>();
	enemy = std::make_shared<Enemy>();

	player->SetEnemy(enemy);
	enemy->SetPlayer(player);
}

void SceneMattsu::Finalize()
{
	//�X�e�[�W�I����
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}
}

void SceneMattsu::Update(float elapsedTime)
{
	GamePad& gamepad = Input::Instance().GetGamePad();

	UpdateCamera(elapsedTime);

	//�X�e�[�W�X�V����
	stage->Update(elapsedTime);
	player->Update(elapsedTime);
	enemy->Update(elapsedTime);

	Collision();

	player->UpdateTransform();
	enemy->UpdateTransform();
}

void SceneMattsu::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();

	Camera::Instance().SetPerspectiveFov(45,
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f);


	// �`�揀��
	RenderContext rc;
	rc.deviceContext = dc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f };	// ���C�g�����i�������j
	rc.renderState = graphics.GetRenderState();

	//�J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	// 3D���f���`��
	{
		//�X�e�[�W�`��
		stage->Render(rc, modelRenderer);

		player->Render(rc, modelRenderer);
		enemy->Render(rc, modelRenderer);
	}

	// 3D�f�o�b�O�`��
	{
		player->RenderDebug(rc, shapeRenderer, { 1,2,1 }, { 1,1,1,1 }, DEBUG_MODE::BOX | DEBUG_MODE::CAPSULE);
		enemy->RenderDebug(rc, shapeRenderer, { 1,2,1 }, { 1,0,0,1 }, DEBUG_MODE::BOX);
	}

	// 2D�X�v���C�g�`��
	{

	}
}

void SceneMattsu::DrawGUI()
{
	player->DrawDebug();
	i_CameraController->DebugGUI();
}

// �V�[�����I�u�W�F�N�g�̓����蔻��
void SceneMattsu::Collision()
{
	PlayerVsStage();
}

void SceneMattsu::PlayerVsStage()
{
	using namespace DirectX;

#define AABB
#ifdef AABB

#endif

#define RAY_CAST
#ifdef RAY_CAST
	const XMFLOAT3 playerPos = player->GetPosition();
	const XMFLOAT3 playerDir = player->GetDirection();
	XMFLOAT3 playerRight;
	const XMFLOAT3 up = { 0,1,0 };
	XMStoreFloat3(&playerRight, XMVector3Cross(XMLoadFloat3(&up), XMLoadFloat3(&playerDir)));


	const XMFLOAT3 rayStart = { playerPos.x, playerPos.y, playerPos.z };
	const XMFLOAT3 rayEndF = { playerPos.x + playerDir.x, playerPos.y, playerPos.z + playerDir.z };
	const XMFLOAT3 rayEndR= { playerPos.x + playerRight.x, playerPos.y, playerPos.z + playerRight.z };

	DirectX::XMFLOAT3 hitPosition, hitNormal;
	if (Collision::RayCast(rayStart, rayEndF, stage->GetWorld(), stage->GetModel(), hitPosition, hitNormal))
	{
		XMVECTOR P = XMLoadFloat3(&hitPosition);
		XMVECTOR E = XMLoadFloat3(&rayEndF);
		XMVECTOR PE = XMVectorSubtract(E, P);

		// �O�p�֐��ŏI�_����ǂ܂ł̒��������߂�
		XMVECTOR N = XMLoadFloat3(&hitNormal);
		// PE�̏I�_��N�x�N�g���������Ă���
		// ���K������N��PE�œ���
		XMVECTOR NegatePE = XMVectorNegate(PE); // ���̂܂�PE��A�œ��ς���Ƃ��������Ȃ����Ⴄ����PE�̋t�x�N�g�������߂�
		N = XMVector3Normalize(N);
		XMVECTOR A = XMVector3Dot(NegatePE, N); // �ˉe�������߂�
		//XMVECTOR A = XMVector3Dot(XMVectorNegate(PE), N);

		// �ǂ܂ł̒������������������Ȃ�悤�ɕ␳����
		float a = XMVectorGetX(A) + 0.01f;

		// �Ǎ���̃x�N�g�������߂�
		A = XMVectorScale(N, a);
		XMVECTOR R = XMVectorAdd(PE , A);
		//XMVECTOR R = XMVectorAdd(PE, XMVectorScale(N, a));
		//XMVECTOR R = XMVector3Dot(XMVectorNegate(PE), N);

		// �Ǎ����̈ʒu�����߂�
		XMVECTOR Q = XMVectorAdd(P , R);
		XMFLOAT3 q;
		XMStoreFloat3(&q, Q);


		//if (Collision::RayCast(rayStart, q, stage->GetWorld(), stage->GetModel(), hitPosition, hitNormal))
		//{
		//	P = XMLoadFloat3(&hitPosition);
		//	XMVECTOR S = XMLoadFloat3(&rayStart);
		//	XMVECTOR PS = XMVectorSubtract(S, P);
		//	XMVECTOR V = XMVector3Normalize(PS);
		//	P = XMVectorAdd(PS, A);
		//	DirectX::XMStoreFloat3(&hitPosition, P);
		//	XMFLOAT3 playerPos = player->GetPosition();
		//	playerPos.x = hitPosition.x;
		//	playerPos.z = hitPosition.z;
		//	player->SetPosition(playerPos);
		//}
		//else
		{
#if 0
			XMFLOAT3 playerPos = player->GetPosition();
			playerPos.x = q.x - playerDir.x;
			playerPos.z = q.z - playerDir.z;
			player->SetPosition(playerPos);
#else
			XMFLOAT3 playerPos = player->GetPosition();
			playerPos.x = q.x - playerDir.x;
			playerPos.z = q.z - playerDir.z;
			player->SetPosition(playerPos);
#endif
		}
	}
#if 0
	else if (Collision::RayCast(rayStart, rayEndR, stage->GetWorld(), stage->GetModel(), hitPos, hitNormal))
	{
		XMVECTOR P = XMLoadFloat3(&hitPos);
		XMVECTOR E = XMLoadFloat3(&rayEndR);
		XMVECTOR PE = XMVectorSubtract(E, P);

		// �O�p�֐��ŏI�_����ǂ܂ł̒��������߂�
		XMVECTOR N = XMLoadFloat3(&hitNormal);
		// PE�̏I�_��N�x�N�g���������Ă���
		// ���K������N��PE�œ���
		XMVECTOR NegatePE = XMVectorNegate(PE); // ���̂܂�PE��A�œ��ς���Ƃ��������Ȃ����Ⴄ����PE�̋t�x�N�g�������߂�
		N = XMVector3Normalize(N);
		XMVECTOR A = XMVector3Dot(NegatePE, N); // �ˉe�������߂�
		//XMVECTOR A = XMVector3Dot(XMVectorNegate(PE), N);

		// �ǂ܂ł̒������������������Ȃ�悤�ɕ␳����
		float a = XMVectorGetX(A) + 0.001f;

		// �Ǎ���̃x�N�g�������߂�
		A = XMVectorScale(N, a);
		XMVECTOR R = PE + A;
		//XMVECTOR R = XMVectorAdd(PE, XMVectorScale(N, a));
		//XMVECTOR R = XMVector3Dot(XMVectorNegate(PE), N);

		// �Ǎ����̈ʒu�����߂�
		XMVECTOR Q = P + R;
		XMFLOAT3 q;
		XMStoreFloat3(&q, Q);


		if (Collision::RayCast(rayStart, q, stage->GetWorld(), stage->GetModel(), hitPos, hitNormal))
		{
			P = XMLoadFloat3(&hitPos);
			XMVECTOR S = XMLoadFloat3(&rayStart);
			XMVECTOR PS = XMVectorSubtract(S, P);
			XMVECTOR V = XMVector3Normalize(PS);
			P = XMVectorAdd(PS, A);
			DirectX::XMStoreFloat3(&hitPos, P);
			XMFLOAT3 playerPos = player->GetPosition();
			playerPos.x = hitPos.x;
			playerPos.z = hitPos.z;
			player->SetPosition(playerPos);
		}
		else
		{
#if 0
			XMFLOAT3 playerPos = player->GetPosition();
			playerPos.x = q.x - playerRight.x; 
			playerPos.z = q.z - playerRight.z; 
			player->SetPosition(playerPos);
#endif
		}
	}
#endif
#endif //end RAY_CAST

#if 1
	XMFLOAT3 start, end;
	start = player->GetPosition();
	end = { start.x , start.y -5.0f, start.z };
	static XMFLOAT3 saveLastPosition;

	//XMFLOAT3 hitPosition, hitNormal;
	if (Collision::RayCast(start, end, stage->GetWorld(), stage->GetModel(), hitPosition, hitNormal))
	{
		XMFLOAT3 playerPosition = start;
		playerPosition.y = hitPosition.y;
		saveLastPosition = playerPosition;
		player->SetPosition(playerPosition);
	}
	else
	{
		//XMFLOAT3 dir = player->GetDirection();
		//saveLastPosition.x += dir.x;
		//saveLastPosition.z += dir.z;
		player->SetPosition(saveLastPosition);
	}
#endif
}

void SceneMattsu::UpdateCamera(float dt)
{
	GamePad& gamepad = Input::Instance().GetGamePad();

	// ��l�̗p�J����
	if (typeid(*i_CameraController) == typeid(FPCameraController))
	{
		// �J�����؂�ւ��@�v���C���[���G(�G���v���C���[)
		bool cameraFlag = player->GetUseCam();

		POINT screenPoint = { Input::Instance().GetMouse().GetScreenWidth() / 2, Input::Instance().GetMouse().GetScreenHeight() / 2 };
		ClientToScreen(Graphics::Instance().GetWindowHandle(), &screenPoint);

		DirectX::XMFLOAT3 cameraPos = {};
		if (!cameraFlag)
		{
			cameraPos = player->GetPosition();
			cameraPos.y = player->GetViewPoint();
		}
		else
		{
			cameraPos = enemy->GetPosition();
			cameraPos.y = enemy->GetViewPoint();
			i_CameraController->SetPitch(enemy->GetPitch());
			i_CameraController->SetYaw(enemy->GetYaw());
		}

		i_CameraController->SetCameraPos(cameraPos);
		i_CameraController->SetUseEnemyCam(cameraFlag); // �v���C���[���J�������g���Ă��邩
		i_CameraController->SetIsChange(player->GetIsChange());
		i_CameraController->Update(dt);
		SetCursorPos(screenPoint.x, screenPoint.y);

		if (gamepad.GetButton() & GamePad::CTRL && gamepad.GetButtonDown() & GamePad::BTN_X)
		{
			i_CameraController = std::make_unique<FreeCameraController>();
		}
	}
	// �t���[�J����
	else
	{
		i_CameraController->Update(dt);

		if (gamepad.GetButton() & GamePad::CTRL && gamepad.GetButtonDown() & GamePad::BTN_X)
		{
			i_CameraController = std::make_unique<FPCameraController>();
		}
		}
}
