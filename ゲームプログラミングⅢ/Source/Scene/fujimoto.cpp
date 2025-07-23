#include "System/Graphics.h"
#include "Camera/Camera.h"
#include "System/GamePad.h"
#include "System/Input.h"
#include"Scene/SceneGameOver.h"
#include"Scene/SceneManager.h"
#include "Collision.h"
#include "./LightModels/LightManager.h"
#include "./Aircon/AirconManager.h"
#include "fujimoto.h"
#include "Pursuer/SearchAI.h"
#include "GameObjects/battery/batteryManager.h"
#include "GameObjects/Object/ObjectManager.h"
#include "imgui.h"                    // ImGui�̊�{�@�\
#include "imgui_impl_win32.h"        // Win32�p�o�b�N�G���h
#include "imgui_impl_dx11.h"         // DirectX11�p�o�b�N�G���h


#include <imgui.h>

CONST LONG SHADOWMAP_WIDTH = { 2048 };
CONST LONG SHADOWMAP_HEIGHT = { 2048 };

// ������
void fujimoto::Initialize()
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
	//i_CameraController = std::make_unique<FPCameraController>();
	i_CameraController = std::make_unique<LightDebugCameraController>();

	player = std::make_shared<Player>();

	enemy = std::make_shared<Enemy>(player, stage);

	//�~�j�}�b�v�X�v���C�g������
	minimap = new MiniMap();
	timer = 0.0f; // �^�C�}�[������
	transTimer = 0.0f; // �V�[���J�ڃ^�C�}�[������

	selectTrans = SelectTrans::GameOver; // �V�[���J�ڑI��������
	sceneTrans = false; // �V�[���J�ڃt���O������

	// shadowMap
	ID3D11Device* device = Graphics::Instance().GetDevice();
	shadow = std::make_unique<ShadowCaster>(device, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);

	// ���C�g�̏�����
	LightManager::Instance().Initialize();

	// �G�A�R���̏�����
	AirconManager::Instance().Initialize();

	batteryManager::Instance().addBattery({ 0,0,0 });
	batteryManager::Instance().SetPlayer_and_enemy(player, enemy);
	batteryManager::Instance().ResetPlayer_get_Batterry();
}

// �I����
void fujimoto::Finalize()
{
	//�X�e�[�W�I����
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}
	//�~�j�}�b�v�I����
	if (minimap != nullptr)
	{
		delete minimap;
		minimap = nullptr;
	}
}

// �X�V����
void fujimoto::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// �C�ӂ̃Q�[���p�b�h�{�^����������Ă��邩
	const GamePadButton anyButton =
		GamePad::BTN_A
		| GamePad::BTN_B
		| GamePad::BTN_X
		| GamePad::BTN_Y;

	bool buttonPressed = (anyButton & gamePad.GetButton()) != 0;
	bool zKey = GetAsyncKeyState('Z') & 0x8000;

	// �t���O���܂������Ă��Ȃ��ꍇ�ɓ��͌��o
	if (!sceneTrans)
	{
		if (zKey)
		{
			nextScene = new Game_Over;
			sceneTrans = true;
			transTimer = 0.0f;
			selectTrans = SelectTrans::GameOver; // �Q�[���I�[�o�[�V�[���ɑJ��
		}

	}
	else
	{
		// �t���O�������Ă���ԃ^�C�}�[�����Z���A1�b�ȏ�o������V�[���؂�ւ�
		transTimer += elapsedTime;
		if (transTimer >= 3.0f && nextScene != nullptr)
		{
			SceneManager::instance().ChangeScene(nextScene);
			nextScene = nullptr; // ���d�J�ږh�~
			sceneTrans = false; // �V�[���J�ڃt���O�����Z�b�g
		}
	}

	timer += elapsedTime;
	Graphics::Instance().UpdateConstantBuffer(timer, transTimer);

	////�Q�[���I�[�o�[�ɋ����J��
	//if (GetAsyncKeyState('Z') & 0x8000)
	//{
	//	// Z�L�[��������Ă���Ƃ��Ɏ��s�����
	//	SceneManager::instance().ChangeScene(new Game_Over);
	//}

	//�X�e�[�W�X�V����
	stage->Update(elapsedTime);
	player->Update(elapsedTime);
	enemy->Update(elapsedTime);
	minimap->Update(player->GetPosition());

	// ��l�̗p�J����
	if (typeid(*i_CameraController) == typeid(FPCameraController))
	{
		POINT screenPoint = { Input::Instance().GetMouse().GetScreenWidth() / 2, Input::Instance().GetMouse().GetScreenHeight() / 2 };
		ClientToScreen(Graphics::Instance().GetWindowHandle(), &screenPoint);
		DirectX::XMFLOAT3 cameraPos = player->GetPosition();
		cameraPos.y = player->GetViewPoint();
		i_CameraController->SetCameraPos(cameraPos);
		i_CameraController->Update(elapsedTime);
		SetCursorPos(screenPoint.x, screenPoint.y);

		if (gamePad.GetButton() & GamePad::CTRL && gamePad.GetButtonDown() & GamePad::BTN_X)
		{
			i_CameraController = std::make_unique<FreeCameraController>();
		}
	}
	// �t���[�J����
	else
	{
		i_CameraController->Update(elapsedTime);

		if (gamePad.GetButton() & GamePad::CTRL && gamePad.GetButtonDown() & GamePad::BTN_X)
		{
			i_CameraController = std::make_unique<FPCameraController>();
		}
	}
	Graphics::Instance().UpdateConstantBuffer(timer, transTimer);


	LightManager::Instance().Update();

	batteryManager::Instance().Update(elapsedTime);

	ObjectManager::Instance().Update(elapsedTime);
}

// �`�揈��
void fujimoto::Render()
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
	// �萔�̍X�V
	UpdateConstants(rc);
	LightManager::Instance().UpdateConstants(rc);
	/*Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->clear(dc, 0.5f, 0.5f, 1, 1);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->activate(dc);*/
	// 3D���f���`��
	{
		//�X�e�[�W�`��
		stage->Render(rc, modelRenderer);

		player->Render(rc, modelRenderer);

		enemy->Render(rc, modelRenderer);

		LightManager::Instance().Render(rc);

		//AirconManager::Instance().Render(rc);

		batteryManager::Instance().Render(rc, modelRenderer);

		ObjectManager::Instance().Render(rc, modelRenderer);
	}

	// 3D�f�o�b�O�`��
	{
		player->RenderDebug(rc, shapeRenderer, { 1,2,1 }, { 1,1,1,1 }, DEBUG_MODE::BOX | DEBUG_MODE::CAPSULE);

		//enemy->RenderDebug(rc, shapeRenderer, { 1,2,1 }, { 1,1,1,1 }, DEBUG_MODE::BOX | DEBUG_MODE::CAPSULE);

	}

	/// �����蔻��̍X�V
	Collision();

	player->UpdateTransform();

	// 2D�X�v���C�g�`��
	{
		//minimap->Render(player->GetPosition());
	}

}

// GUI�`��
void fujimoto::DrawGUI()
{
	minimap->DrawImGui();
	LightManager::Instance().DebugGUI();
	RenderContext rc;

	ImGui::Separator();

	// (ToT)
	ImGui::SliderFloat3("lightDirection", reinterpret_cast<float*>(&lightDirection), -1.0f, +1.0f);
	ImGui::DragFloat("shadowMapDrawRect", &SHADOWMAP_DRAWRECT, 0.1f);

	// shadow->DrawGUI();

	ImGui::Separator();

	if (ImGui::TreeNode("texture"))
	{
		ImGui::Text("shadow_map");
		//ImGui::Image(shadowShaderResourceView.Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::DragFloat("shadowBias", &shadowBias, 0.0001f, 0, 1, "%.6f");
		ImGui::ColorEdit3("shadowColor", reinterpret_cast<float*>(&shadowColor));

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("ambient"))
	{
		ImGui::InputFloat3("cameraPosition", &cameraPosition.x);
		ImGui::ColorEdit3("ambient_color", &ambientColor.x);
		ImGui::ColorEdit3("fog_color", &fogColor.x);
		ImGui::DragFloat("fog_near", &fogRange.x, 0.1f, +100.0f);
		ImGui::DragFloat("fog_far", &fogRange.y, 0.1f, +100.0f);


		ImGui::TreePop();
	}/*
	Graphics::Instance().DebugGUI();
	LightManager::Instance().DebugGUI();
	AirconManager::Instance().DebugGUI();*/
	ImGui::Begin("Scene Info");

	// position��\��
	ImGui::Text("Position: X=%.2f, Y=%.2f, Z=%.2f",
		player->GetPosition().x,
		player->GetPosition().y,
		player->GetPosition().z);

	ImGui::Text("index: X=%d",
		enemy.get()->GetindexWayPoint());


	/*ImGui::Text("Position: X=%.2f, Y=%.2f, Z=%.2f",
		i_CameraController->GetTarget().x,
		i_CameraController->GetTarget().y,
		i_CameraController->GetTarget().z);*/
	ImGui::End();
}

void fujimoto::Collision()
{
	/// �v���C���[�ƃX�e�[�W�Ƃ̓����蔻��
	PlayerVsStage();
}

void fujimoto::PlayerVsStage()
{
	using namespace DirectX;

	const XMFLOAT3 playerPos = player->GetPosition();
	const XMFLOAT3 playerDir = player->GetDirection();
	XMFLOAT3 playerRight;
	const XMFLOAT3 up = { 0,1,0 };
	XMStoreFloat3(&playerRight, XMVector3Cross(XMLoadFloat3(&up), XMLoadFloat3(&playerDir)));


	const XMFLOAT3 rayStart = { playerPos.x, playerPos.y, playerPos.z };
	const XMFLOAT3 rayEndF = { playerPos.x + playerDir.x, playerPos.y, playerPos.z + playerDir.z };
	const XMFLOAT3 rayEndR = { playerPos.x + playerRight.x, playerPos.y, playerPos.z + playerRight.z };

	DirectX::XMFLOAT3 hitPosition, hitNormal;
	if (Collision::RayCast(rayStart, rayEndF, stage->GetCollisionMatrix(), stage->GetCollisionMesh(), hitPosition, hitNormal))
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
		XMVECTOR R = XMVectorAdd(PE, A);
		//XMVECTOR R = XMVectorAdd(PE, XMVectorScale(N, a));
		//XMVECTOR R = XMVector3Dot(XMVectorNegate(PE), N);

		// �Ǎ����̈ʒu�����߂�
		XMVECTOR Q = XMVectorAdd(P, R);
		XMFLOAT3 q;
		XMStoreFloat3(&q, Q);

		XMFLOAT3 playerPos = player->GetPosition();
#if 0
		playerPos.x = q.x - playerDir.x;
		playerPos.z = q.z - playerDir.z;
#else
		playerPos.x = q.x;
		playerPos.z = q.z;
#endif
		player->SetPosition(playerPos);
	}
}

void fujimoto::UpdateCamera(float elapsedTime)
{
}

void fujimoto::UpdateConstants(RenderContext& rc)
{
	rc.lightDirection = lightDirection;	// (ToT)+
	// �V���h�E�̐ݒ�
	rc.shadowColor = shadowColor;
	rc.shadowBias = shadowBias;

	// �t�H�O�̐ݒ�
	rc.ambientColor = ambientColor;
	rc.fogColor = fogColor;
	rc.fogRange = fogRange;

	//�J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();
	cameraPosition = camera.GetEye();
	rc.cameraPosition.x = cameraPosition.x;
	rc.cameraPosition.y = cameraPosition.y;
	rc.cameraPosition.z = cameraPosition.z;

	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();
}
