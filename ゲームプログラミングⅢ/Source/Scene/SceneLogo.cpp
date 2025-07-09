#include"System/Graphics.h"
#include"System/Input.h"
#include"SceneLogo.h"
#include"SceneManager.h"

//������
void SceneLogo::Initialize()
{
	//�X�v���C�g������
	sprite = new Sprite("Data/Sprite/LoadingIcon.png");

	//�X���b�h�J�n
	thread = new std::thread(LoadingThread, this);
	timer = 0.0f;
	transtimer = 0.0f;
	nextSceneReadyTime = -1.0f; // ���̃V�[���̏�������������������
}

//�I����
void SceneLogo::Finalize()
{
	//�X���b�h�I����
	if (thread != nullptr)
	{
		thread->join();
		delete thread;
		thread = nullptr;
	}

	//�X�v���C�g�I����
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
}

//�X�V����
void SceneLogo::Update(float elapsedTime)
{
	constexpr float speed = 180.0f;
	angle += speed * elapsedTime;
	positionX -= 10.0f * elapsedTime;

	// �o�ߎ��Ԃ����Z
	timer += elapsedTime;

	if (nextScene->IsReady())
	{
		// �����������̎������L�^�i1�񂾂��j
		if (nextSceneReadyTime < 0.0f)
		{
			nextSceneReadyTime = timer;
		}

		// ������������2�b�o�߂�����V�[���J��
		if ((timer - nextSceneReadyTime) >= 2.0f)
		{
			SceneManager::instance().ChangeScene(nextScene);
		}
		transtimer += elapsedTime;
	}
	Graphics::Instance().UpdateConstantBuffer(timer, transtimer);
}

//�`�揈��
void SceneLogo::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//�`�揀��
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();


	/// �t���[���o�b�t�@�̃N���A�ƃA�N�e�B�x�[�g�i�|�X�g�v���Z�X�p�j
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->clear(dc, 0, 0, 0, 1);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->activate(dc);



	/// �t���[���o�b�t�@�̃f�B�A�N�e�B�x�[�g
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->deactivate(dc);
	// GameOver


	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::GameOver)].Get());


	////2D�X�v���C�g�`��
	{
		//��ʉE���Ƀ��[�f�B���O�A�C�R����`��
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float spriteWidth = 256;
		float spriteHeight = 256;
		positionX = screenWidth - spriteWidth;
		positionY = screenHeight - spriteHeight;

		sprite->Render(rc,
			positionX, positionY, 0, spriteWidth, spriteHeight,
			angle,
			1, 1, 1, 1);
	}
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->deactivate(dc);


	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);

	//vIsionBootDown
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::VisionBootDown)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->deactivate(dc);

	//fAdeToBlack
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FadeToBlack)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FadeToBlack)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::FadeToBlack)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FadeToBlack)]->deactivate(dc);


	Graphics::Instance().bit_block_transfer->blit(
		dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FadeToBlack)]->shader_resource_views[0].GetAddressOf(), 10, 1
	);


}

//GUI�`��
void SceneLogo::DrawGUI()
{

}

//���[�f�B���O�X���b�h
void SceneLogo::LoadingThread(SceneLogo* scene)
{
	//COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
	CoInitialize(nullptr);

	//���̃V�[���̏��������s��
	scene->nextScene->Initialize();

	//�X���b�h���I���O��COM�֘A�̏I����
	CoUninitialize();

	//���̃V�[���̏��������ݒ�
	scene->nextScene->SetReady();
}
