#include "System/Graphics.h"
#include "SceneGraphics.h"
#include "Camera/Camera.h"
#include "System/GamePad.h"
#include "System/Input.h"
#include "./LightModels/LightManager.h"
#include "Aircon/AirconManager.h"
#include "TV/TV.h"
#include "Object/ObjectManager.h"

#include <imgui.h>

CONST LONG SHADOWMAP_WIDTH = { 2048 };
CONST LONG SHADOWMAP_HEIGHT = { 2048 };



/**
 * @file SceneGraphics.cpp
 * @brief �V�[���O���t�B�b�N�X�̏������A�X�V�A�`��AGUI�`����s���N���X�̎����t�@�C��
 * @author
 * @date
 *
 * �{�t�@�C���ł́A�V�[�����̃O���t�B�b�N�X�Ɋւ����v�ȏ����i�������A�I�����A�X�V�A�`��AGUI�`��j���������Ă��܂��B
 * �e�֐��ɂ�Doxygen�`���ŏڍׂȐ������L�ڂ��Ă��܂��B
 *
 * @see SceneGraphics.h
 */

 /**
  * @brief �V�[���O���t�B�b�N�X�̏������������s���܂��B
  *
  * �X�e�[�W�A�J�����A�J�����R���g���[���[�A�v���C���[�̏��������s���܂��B
  *
  * @details
  * - �X�e�[�W�̃C���X�^���X�𐶐����܂��B
  * - �J�����̏����ʒu�E�����_�E�������ݒ肵�܂��B
  * - ��l�̃J�����R���g���[���[�iFPCameraController�j�𐶐����܂��B
  * - �v���C���[�̃C���X�^���X�𐶐����܂��B
  *
  * @see Stage, Camera, FPCameraController, Player
  */
void SceneGraphics::Initialize()
{
	/// �X�e�[�W�̃C���X�^���X����
	stage = new Stage();

	/// �J�����̏����ݒ�
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10), ///< �J�����̏����ʒu
		DirectX::XMFLOAT3(0, 0, 0),    ///< �����_
		DirectX::XMFLOAT3(0, 1, 0)     ///< ������x�N�g��
	);

	/// ��l�̃J�����R���g���[���[�̐���
	i_CameraController = std::make_unique<FPCameraController>();

	/// �v���C���[�̃C���X�^���X����
	player = std::make_shared<Player>();

	// shadowMap
	ID3D11Device* device = Graphics::Instance().GetDevice();
	shadow = std::make_unique<ShadowCaster>(device, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);

	// �Q�[���I�u�W�F�N�g�̏�����
	LightManager::Instance().Initialize();
	AirconManager::Instance().Initialize();
	//TV::Instance().Initialize();
	ObjectManager::Instance().Initialize();

	/// �f�o�b�O�p�^�C�}�[������
	time = 0;
	// 3D�I�[�f�B�I�V�X�e���ɃG�~�b�^�[��ǉ�
	//BGM


	// ���X�i�[�̏����ʒu�ƌ�����ݒ�
	Audio3DSystem::Instance().UpdateListener(Camera::Instance().GetEye(), Camera::Instance().GetFront(), Camera::Instance().GetUp());
	// 3D�I�[�f�B�I�V�X�e���̍Đ��J�n
	Audio3DSystem::Instance().PlayByTag("atmosphere_noise");
	Audio3DSystem::Instance().PlayByTag("aircon");
}

/**
 * @brief �V�[���O���t�B�b�N�X�̏I�����������s���܂��B
 *
 * �X�e�[�W�̃�����������s���܂��B
 *
 * @details
 * - �X�e�[�W�����݂���ꍇ��delete���A�|�C���^��nullptr�ɂ��܂��B
 *
 * @see Stage
 */
void SceneGraphics::Finalize()
{
	/// �X�e�[�W�̃��������
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}
	Audio3DSystem::Instance().StopByTag("atmosphere_noise"); // ������~
	Audio3DSystem::Instance().StopByTag("aircon"); // ������~
}

/**
 * @brief �V�[���O���t�B�b�N�X�̍X�V�������s���܂��B
 *
 * �X�e�[�W�A�v���C���[�A�J�����R���g���[���[�̍X�V���s���A�J�����̐؂�ւ����Ǘ����܂��B
 *
 * @param elapsedTime �O�t���[������̌o�ߎ��ԁi�b�j
 *
 * @details
 * - ���̓f�o�C�X�̎擾
 * - �X�e�[�W�ƃv���C���[�̍X�V
 * - �J�����R���g���[���[�̎�ނɉ����ăJ�����̍X�V�Ɛ؂�ւ�
 * - �O���t�B�b�N�X�̒萔�o�b�t�@�X�V
 *
 * @see Stage::Update, Player::Update, ICameraController, FPCameraController, FreeCameraController
 */
void SceneGraphics::Update(float elapsedTime)
{
	//Sleep(10);


	GamePad& gamepad = Input::Instance().GetGamePad(); ///< �Q�[���p�b�h����

	/// �X�e�[�W�ƃv���C���[�̍X�V
	stage->Update(elapsedTime);
	player->Update(elapsedTime);

	/// �J�����R���g���[���[�̎�ނɂ�镪��
	// ��l�̃J�����R���g���[���[�̏ꍇ
	if (typeid(*i_CameraController) == typeid(FPCameraController))
	{
		//i_CameraController = std::make_unique<LightDebugCameraController>();
#if 1
		/// ��ʒ����̍��W���擾���A�}�E�X�J�[�\���𒆉��Ɉړ�
		POINT screenPoint = { Input::Instance().GetMouse().GetScreenWidth() / 2, Input::Instance().GetMouse().GetScreenHeight() / 2 };
		ClientToScreen(Graphics::Instance().GetWindowHandle(), &screenPoint);

		/// �v���C���[�̈ʒu���J�����ʒu�ɐݒ�
		DirectX::XMFLOAT3 cameraPos = player->GetPosition();
		cameraPos.y = 1.5f;
		i_CameraController->SetCameraPos(cameraPos);

		/// �J�����R���g���[���[�̍X�V
		i_CameraController->Update(elapsedTime);

		/// �}�E�X�J�[�\������ʒ����Ɉړ�
		SetCursorPos(screenPoint.x, screenPoint.y);

		/// CTRL+X�{�^���Ńt���[�J�����ɐ؂�ւ�
		if (gamepad.GetButton() & GamePad::CTRL && gamepad.GetButtonDown() & GamePad::BTN_X)
		{
			//i_CameraController = std::make_unique<FreeCameraController>();
			i_CameraController = std::make_unique<LightDebugCameraController>();
		}
#endif
	}
	// �t���[�J�����R���g���[���[�̏ꍇ
	else
	{
		/// �J�����R���g���[���[�̍X�V
		i_CameraController->Update(elapsedTime);

		/// CTRL+X�{�^���ň�l�̃J�����ɐ؂�ւ�
		if (gamepad.GetButton() & GamePad::CTRL && gamepad.GetButtonDown() & GamePad::BTN_X)
		{
			i_CameraController = std::make_unique<FPCameraController>();
		}
	}

	/// �O���t�B�b�N�X�̒萔�o�b�t�@�X�V
	/// ���Ԃ̍X�V�i�f�o�b�O�p�j
	time += elapsedTime; ///< �~���b�P�ʂŎ��Ԃ��X�V

	Graphics::Instance().UpdateConstantBuffer(time);

	LightManager::Instance().Update();
	//TV::Instance().Update(elapsedTime);
	ObjectManager::Instance().Update(elapsedTime);

	Audio3DSystem::Instance().SetEmitterPositionByTag("atmosphere_noise", Camera::Instance().GetEye());
	Audio3DSystem::Instance().UpdateListener(Camera::Instance().GetEye(), Camera::Instance().GetFront(), Camera::Instance().GetUp());
	Audio3DSystem::Instance().UpdateEmitters(elapsedTime);
}

/**
 * @brief �V�[���O���t�B�b�N�X�̕`�揈�����s���܂��B
 *
 * �X�e�[�W�A�v���C���[��3D���f���`��A�f�o�b�O�`��A�|�X�g�v���Z�X�������s���܂��B
 *
 * @details
 * - �J�����̎ˉe�s��ݒ�
 * - �`��p�R���e�L�X�g(RenderContext)�̏���
 * - �X�e�[�W�E�v���C���[��3D���f���`��
 * - �v���C���[�̃f�o�b�O�`��
 * - �|�X�g�v���Z�X�i�n�C���C�g�A�u���[���j�̓K�p
 *
 * @see RenderContext, ModelRenderer, ShapeRenderer, Camera, Player, Stage
 */
void SceneGraphics::Render()
{
	Graphics& graphics = Graphics::Instance(); ///< �O���t�B�b�N�X�Ǘ��C���X�^���X
	ID3D11DeviceContext* dc = graphics.GetDeviceContext(); ///< �f�o�C�X�R���e�L�X�g
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer(); ///< �`��`��p�����_���[
	ModelRenderer* modelRenderer = graphics.GetModelRenderer(); ///< ���f���`��p�����_���[

	/// �J�����̎ˉe�s���ݒ�
	Camera::Instance().SetPerspectiveFov(
		45, ///< ����p�i�x�j
		graphics.GetScreenWidth() / graphics.GetScreenHeight(), ///< �A�X�y�N�g��
		0.1f, ///< �j�A�N���b�v
		1000.0f ///< �t�@�[�N���b�v
	);

	/// �`��p�R���e�L�X�g�̏���
	RenderContext rc;
	rc.deviceContext = dc; ///< �f�o�C�X�R���e�L�X�g
	rc.lightDirection = lightDirection; ///< ���C�g�̕����i�������j
	rc.renderState = graphics.GetRenderState(); ///< �����_�[�X�e�[�g

	/// �J�����p�����[�^�̐ݒ�
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView(); ///< �r���[�s��
	rc.projection = camera.GetProjection(); ///< �ˉe�s��
#if 0
	// shadow
	{
		Camera& camera = Camera::Instance();

		// ���C�g�̈ʒu���猩�������s��𐶐�
		DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&lightDirection); // TODO : pointLight�ɂ���Ƃ��͂�����ύX
		LightPosition = DirectX::XMVectorScale(LightPosition, -50);
		DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(LightPosition,
			DirectX::XMVectorSet(camera.GetFocus().x, camera.GetFocus().y, camera.GetFocus().z, 1.0f), // (ToT)
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		// �V���h�E�}�b�v�ɕ`�悵�����͈͂̎ˉe�s��𐶐�
		DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(SHADOWMAP_DRAWRECT, SHADOWMAP_DRAWRECT,
			0.1f, 200.0f);

		DirectX::XMStoreFloat4x4(&rc.view, V);
		DirectX::XMStoreFloat4x4(&rc.projection, P);
		DirectX::XMStoreFloat4x4(&rc.lightViewProjection, V * P);

		shadow->Clear(dc, 1.0f);
		shadow->Active(dc);

		// 3D���f���`��
		{
			/// �X�e�[�W�̕`��
			//stage->Render(rc, modelRenderer);
		}
		shadow->Deactive(dc);
	}
#endif
	// �萔�̍X�V
	UpdateConstants(rc);
	LightManager::Instance().UpdateConstants(rc);

	/// �t���[���o�b�t�@�̃N���A�ƃA�N�e�B�x�[�g�i�|�X�g�v���Z�X�p�j
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->clear(dc, 0.5f, 0.5f, 1, 1);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->activate(dc);

	/// 3D���f���`�揈��
	{
		/// �X�e�[�W�̕`��
		stage->Render(rc, modelRenderer);

		/// �v���C���[�̕`��
		player->Render(rc, modelRenderer);

		/// ���C�g���f���̕`��
		LightManager::Instance().Render(rc);

		AirconManager::Instance().Render(rc);

		//TV::Instance().Render(rc, modelRenderer);

		ObjectManager::Instance().Render(rc, modelRenderer);
	}

	/// 3D�f�o�b�O�`�揈��
	{
		/// �v���C���[�̃f�o�b�O�`��i�{�b�N�X�E�J�v�Z���\���j
		player->RenderDebug(rc, shapeRenderer, { 1,2,1 }, { 1,1,1,1 }, DEBUG_MODE::BOX | DEBUG_MODE::CAPSULE);

		/// ���C�g���f���̃f�o�b�O�`��
		//LightManager::Instance().RenderDebugPrimitive(rc);

		/// �G�A�R���̃f�o�b�O�`��
		//AirconManager::Instance().RenderDebugPrimitive(rc);

		ObjectManager::Instance().RenderDebugPrimitive(rc, shapeRenderer);
	}

	/// 2D�X�v���C�g�`�揈���i�������j
	{
		// ������2D�X�v���C�g�`�揈����ǉ��\
	}

	//shadow->Release(dc);

	/// �t���[���o�b�t�@�̃f�B�A�N�e�B�x�[�g
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->deactivate(dc);
#if 0
	/// �n�C���C�g�p�X�p�t���[���o�b�t�@�̃N���A�ƃA�N�e�B�x�[�g
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::HighLightPass)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::HighLightPass)]->activate(dc);

	/// �n�C���C�g�p�X�̃|�X�g�v���Z�X�K�p
	Graphics::Instance().bit_block_transfer->blit(
		dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->shader_resource_views[0].GetAddressOf(),
		10, 1,
		Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::HighLightPass)].Get()
	);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::HighLightPass)]->deactivate(dc);

	/// �u���[�p�X�̃|�X�g�v���Z�X�K�p
	ID3D11ShaderResourceView* shader_resource_views[2] = {
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->shader_resource_views[0].Get(),
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::HighLightPass)]->shader_resource_views[0].Get()
	};

	Graphics::Instance().bit_block_transfer->blit(
		dc,
		shader_resource_views, 10, 2,
		Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::Blur)].Get()
	);

	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BreathShake)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BreathShake)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		&shader_resource_views[0], 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::BreathShake)].Get());

	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BreathShake)]->deactivate(dc);

	//TEMPORAL NOISE
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BreathShake)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::TemporalNoise)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->deactivate(dc);

	//// WardenGaze
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::WardenGaze)]->clear(dc);
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::WardenGaze)]->activate(dc);
	//Graphics::Instance().bit_block_transfer->blit(dc,
	//	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::WardenGaze)].Get());
	//{

	//	sprite->Render(rc, 0, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
	//}
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::WardenGaze)]->deactivate(dc);


	//// Sharpen
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Sharpen)]->clear(dc);
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Sharpen)]->activate(dc);
	//Graphics::Instance().bit_block_transfer->blit(dc,
	//	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::Sharpen)].Get());
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Sharpen)]->deactivate(dc);


	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);

	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);


#endif
#if 0
	/// �f�o�b�O�p�F�|�X�g�v���Z�X���ʂ̕`��
	Graphics::Instance().bit_block_transfer->blit(
		dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1
	);
#endif

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
	////TemporalNoise
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->clear(dc);
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->activate(dc);
	//Graphics::Instance().bit_block_transfer->blit(dc,
	//	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BloomFinal)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::TemporalNoise)].Get());
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->deactivate(dc);
	////FilmGrainDustPS
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->clear(dc);
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->activate(dc);
	//Graphics::Instance().bit_block_transfer->blit(dc,
	//	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::FilmGrainDust)].Get());
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->deactivate(dc);

	////crt
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);

	//Graphics::Instance().bit_block_transfer->blit(dc,
	//	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);

	////VisionBootDown
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->clear(dc);
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->activate(dc);
	//Graphics::Instance().bit_block_transfer->blit(dc,
	//	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::VisionBootDown)].Get());
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->deactivate(dc);
	//TVNoiseFade
	/*Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::TVNoiseFade)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->deactivate(dc);*/

	//// GameOver
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->clear(dc);
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->activate(dc);
	//Graphics::Instance().bit_block_transfer->blit(dc,
	//	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::GameOver)].Get());
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->deactivate(dc);


	//LightFlicker
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::LightFlicker)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::LightFlicker)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BloomFinal)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::LightFlicker)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::LightFlicker)]->deactivate(dc);

	//crt

	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::LightFlicker)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);
	////NoiseChanse
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoiseChange)]->clear(dc);
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoiseChange)]->activate(dc);
	//Graphics::Instance().bit_block_transfer->blit(dc,
	//	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::NoiseChange)].Get());
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoiseChange)]->deactivate(dc);
	Graphics::Instance().bit_block_transfer->blit(
		dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1


	);

}

/**
 * @brief �V�[���O���t�B�b�N�X��GUI�`�揈�����s���܂��B
 *
 * @details
 * ����͖������ł��BImGui����GUI�`�揈����ǉ�����ꍇ�͖{�֐��Ɏ������Ă��������B
 */
void SceneGraphics::DrawGUI()
{
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
	AirconManager::Instance().DebugGUI();
	ObjectManager::Instance().DebugGUI();
}

void SceneGraphics::UpdateConstants(RenderContext& rc)
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
