#include "SceneMattsu.h"
#include "System/Graphics.h"
#include "Camera/Camera.h"
#include "System/GamePad.h"
#include "System/Input.h"

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

	//�X�e�[�W�X�V����
	stage->Update(elapsedTime);
	player->Update(elapsedTime);
	enemy->Update(elapsedTime);

	// ��l�̗p�J����
	if (typeid(*i_CameraController) == typeid(FPCameraController))
	{
		// �J�����؂�ւ��@�v���C���[���G(�G���v���C���[)
		bool cameraFlag = player->GetIsChangeCamera();

		POINT screenPoint = { Input::Instance().GetMouse().GetScreenWidth() / 2, Input::Instance().GetMouse().GetScreenHeight() / 2 };
		ClientToScreen(Graphics::Instance().GetWindowHandle(), &screenPoint);

		DirectX::XMFLOAT3 cameraPos = {};
		if (!cameraFlag)
			cameraPos = player->GetPosition();
		else
			cameraPos = enemy->GetPosition();

		i_CameraController->SetCameraPos(cameraPos);
		i_CameraController->Update(elapsedTime);
		SetCursorPos(screenPoint.x, screenPoint.y);

		if (gamepad.GetButton() & GamePad::CTRL && gamepad.GetButtonDown() & GamePad::BTN_X)
		{
			i_CameraController = std::make_unique<FreeCameraController>();
		}
	}
	// �t���[�J����
	else
	{
		i_CameraController->Update(elapsedTime);

		if (gamepad.GetButton() & GamePad::CTRL && gamepad.GetButtonDown() & GamePad::BTN_X)
		{
			i_CameraController = std::make_unique<FPCameraController>();
		}
	}
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
}
