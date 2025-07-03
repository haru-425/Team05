#include "fujimoto.h"
#include "System/Graphics.h"
#include "Camera/Camera.h"
#include "System/GamePad.h"
#include "System/Input.h"
#include "Pursuer/SearchAI.h"
#include "imgui.h"                    // ImGui�̊�{�@�\
#include "imgui_impl_win32.h"        // Win32�p�o�b�N�G���h
#include "imgui_impl_dx11.h"         // DirectX11�p�o�b�N�G���h

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
	i_CameraController = std::make_unique<FPCameraController>();

	player = std::make_shared<Player>();

	enemy = std::make_shared<Enemy>(player, stage);
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
}

// �X�V����
void fujimoto::Update(float elapsedTime)
{
	GamePad& gamepad = Input::Instance().GetGamePad();

	//�X�e�[�W�X�V����
	stage->Update(elapsedTime);
	player->Update(elapsedTime);
	enemy->Update(elapsedTime);

	// ��l�̗p�J����
	if (typeid(*i_CameraController) == typeid(FPCameraController))
	{
		POINT screenPoint = { Input::Instance().GetMouse().GetScreenWidth() / 2, Input::Instance().GetMouse().GetScreenHeight() / 2 };
		ClientToScreen(Graphics::Instance().GetWindowHandle(), &screenPoint);
		DirectX::XMFLOAT3 cameraPos = player->GetPosition();
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
	}

	// 2D�X�v���C�g�`��
	{

	}

	{
		ImGui::Begin("Player Info");

		// position��\��
		ImGui::Text("Position: X=%.2f, Y=%.2f, Z=%.2f",
			player->GetPosition().x,
			player->GetPosition().y,
			player->GetPosition().z);

		ImGui::Text("index: X=%d",
			enemy.get()->GetindexWayPoint());
		ImGui::End();

	}
}

// GUI�`��
void fujimoto::DrawGUI()
{

}
