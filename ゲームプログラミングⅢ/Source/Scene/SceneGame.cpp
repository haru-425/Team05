#include "System/Graphics.h"
#include "SceneGame.h"
#include "Camera/Camera.h"
#include "System/GamePad.h"
#include "System/Input.h"
#include"Scene/SceneGameOver.h"
#include"Scene/SceneManager.h"
#include "../LightManager.h"

#include <imgui.h>

CONST LONG SHADOWMAP_WIDTH = { 2048 };
CONST LONG SHADOWMAP_HEIGHT = { 2048 };

// ������
void SceneGame::Initialize()
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

	//�~�j�}�b�v�X�v���C�g������
	minimap = new MiniMap();
	timer = 0.0f; // �^�C�}�[������
	transTimer = 0.0f; // �V�[���J�ڃ^�C�}�[������

	selectTrans = SelectTrans::GameOver; // �V�[���J�ڑI��������
	sceneTrans = false; // �V�[���J�ڃt���O������

	// shadowMap
	ID3D11Device* device = Graphics::Instance().GetDevice();
	shadow = std::make_unique<ShadowCaster>(device, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);

	LightManager::Instance().Initialize();
}

// �I����
void SceneGame::Finalize()
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
void SceneGame::Update(float elapsedTime)
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
	minimap->Update(player->GetPosition());

	// ��l�̗p�J����
	if (typeid(*i_CameraController) == typeid(FPCameraController))
	{
		POINT screenPoint = { Input::Instance().GetMouse().GetScreenWidth() / 2, Input::Instance().GetMouse().GetScreenHeight() / 2 };
		ClientToScreen(Graphics::Instance().GetWindowHandle(), &screenPoint);
		DirectX::XMFLOAT3 cameraPos = player->GetPosition();
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
}

// �`�揈��
void SceneGame::Render()
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
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->clear(dc, 0.5f, 0.5f, 1, 1);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->activate(dc);
	// 3D���f���`��
	{
		//�X�e�[�W�`��
		stage->Render(rc, modelRenderer);

		player->Render(rc, modelRenderer);
	}

	// 3D�f�o�b�O�`��
	{
		player->RenderDebug(rc, shapeRenderer, { 1,2,1 }, { 1,1,1,1 }, DEBUG_MODE::BOX | DEBUG_MODE::CAPSULE);

		LightManager::Instance().RenderDebug(rc);
	}

	// 2D�X�v���C�g�`��
	{
		//minimap->Render(player->GetPosition());
	}
	/// �t���[���o�b�t�@�̃f�B�A�N�e�B�x�[�g
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->deactivate(dc);
	if (player->GetUseCam())
	{
		//enemy

	// BLOOM
		Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::BloomFinal]->clear(dc);
		Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::BloomFinal]->activate(dc);
		Graphics::Instance().bloomer->make(dc, Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::screenquad]->shader_resource_views[0].Get());

		ID3D11ShaderResourceView* shader_resource_views[] =
		{
			Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::screenquad]->shader_resource_views[0].Get(),
			Graphics::Instance().bloomer->shader_resource_view(),
		};
		Graphics::Instance().bit_block_transfer->blit(dc, shader_resource_views, 10, 2, Graphics::Instance().pixel_shaders[(int)Graphics::PPShaderType::BloomFinal].Get());
		Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::BloomFinal]->deactivate(dc);
		//TemporalNoise
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->activate(dc);
		Graphics::Instance().bit_block_transfer->blit(dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BloomFinal)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::TemporalNoise)].Get());
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->deactivate(dc);
		//FilmGrainDustPS
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->activate(dc);
		Graphics::Instance().bit_block_transfer->blit(dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::FilmGrainDust)].Get());
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->deactivate(dc);
		//WardenGaze
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::WardenGaze)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::WardenGaze)]->activate(dc);
		Graphics::Instance().bit_block_transfer->blit(dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::WardenGaze)].Get());
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::WardenGaze)]->deactivate(dc);

		//crt
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);

		Graphics::Instance().bit_block_transfer->blit(dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);



		Graphics::Instance().bit_block_transfer->blit(
			dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1


		);



	}
	else//player
	{
		// BLOOM
		Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::BloomFinal]->clear(dc);
		Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::BloomFinal]->activate(dc);
		Graphics::Instance().bloomer->make(dc, Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::screenquad]->shader_resource_views[0].Get());

		ID3D11ShaderResourceView* shader_resource_views[] =
		{
			Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::screenquad]->shader_resource_views[0].Get(),
			Graphics::Instance().bloomer->shader_resource_view(),
		};
		Graphics::Instance().bit_block_transfer->blit(dc, shader_resource_views, 10, 2, Graphics::Instance().pixel_shaders[(int)Graphics::PPShaderType::BloomFinal].Get());
		Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::BloomFinal]->deactivate(dc);


		//BreathShake
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BreathShake)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BreathShake)]->activate(dc);
		Graphics::Instance().bit_block_transfer->blit(dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BloomFinal)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::BreathShake)].Get());
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BreathShake)]->deactivate(dc);

		//VisionBootDown
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->activate(dc);
		Graphics::Instance().bit_block_transfer->blit(dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BreathShake)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::VisionBootDown)].Get());
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->deactivate(dc);


		//FadeToBlack
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FadeToBlack)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FadeToBlack)]->activate(dc);
		Graphics::Instance().bit_block_transfer->blit(dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::FadeToBlack)].Get());
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FadeToBlack)]->deactivate(dc);
		//TVNoiseFade
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->activate(dc);
		Graphics::Instance().bit_block_transfer->blit(dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::TVNoiseFade)].Get());
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->deactivate(dc);



		//crt
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);

		switch (selectTrans)
		{
		case SceneGame::SelectTrans::Clear:
			Graphics::Instance().bit_block_transfer->blit(dc,
				Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FadeToBlack)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());

			break;
		case SceneGame::SelectTrans::GameOver:
			Graphics::Instance().bit_block_transfer->blit(dc,
				Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());

			break;
		case SceneGame::SelectTrans::cnt:
			break;
		default:
			break;
		}

		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);



		Graphics::Instance().bit_block_transfer->blit(
			dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1


		);


	}
}

// GUI�`��
void SceneGame::DrawGUI()
{
	minimap->DrawImGui();
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
	}
	Graphics::Instance().DebugGUI();
	LightManager::Instance().DebugGUI();
}
void SceneGame::UpdateConstants(RenderContext& rc)
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
