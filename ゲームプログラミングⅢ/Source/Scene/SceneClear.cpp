#include "SceneClear.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneTitle.h"
#include "System/Audio.h"
#include "System/Input.h"

void Game_Clear::Initialize()
{
	s_rank = new Sprite("Data/Sprite/rank.png");
	s_result = new Sprite("Data/Sprite/result.png");
	s_bg1 = new Sprite("Data/Sprite/rankbg1.png");
	s_bg2 = new Sprite("Data/Sprite/rankbg2.png");
	timer = 0.0f; // �^�C�}�[������
	transTimer = 0.0f; // �V�[���J�ڃ^�C�}�[������
	sceneTrans = false; // �V�[���J�ڃt���O������
	//RankSystem::Instance().SetRank(1, 1, 3);
	result = RankSystem::Instance().GetRank();
	angle = 0;

	// SE�̓ǂݍ���
	selectSE = Audio::Instance().LoadAudioSource("Data/Sound/selectButton.wav");
}

void Game_Clear::Finalize()
{

	if (s_rank != nullptr)
	{
		delete s_rank;
		s_rank = nullptr;
	}
	if (s_bg2 != nullptr)
	{
		delete s_bg2;
		s_bg2 = nullptr;
	}
	if (s_bg1 != nullptr)
	{
		delete s_bg1;
		s_bg1 = nullptr;
	}

	if (s_result != nullptr)
	{
		delete s_result;
		s_result = nullptr;
	}
	GameCleartime = 0.0f; ///< �f�o�b�O�p�^�C�}�[������


	timer = 0;

	if (selectSE != nullptr)
	{
		delete selectSE;
		selectSE = nullptr;
	}
}

void Game_Clear::Update(float elapsedTime)
{
	Mouse& mouse = Input::Instance().GetMouse();

	if(mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		GameCleartime = 120.0f;
		selectSE->Play(false);
	}
	if (GameCleartime >= 120.0f) {
		if (!sceneTrans)
		{

			nextScene = new SceneTitle;
			sceneTrans = true;
			transTimer = 0.0f;
		}
		else
		{
			// �t���O�������Ă���ԃ^�C�}�[�����Z���A1�b�ȏ�o������V�[���؂�ւ�
			transTimer += elapsedTime;
			if (transTimer >= 3.0f && nextScene != nullptr)
			{
				SceneManager::instance().ChangeScene(new SceneLoading(nextScene));
				nextScene = nullptr; // ���d�J�ږh�~
				sceneTrans = false; // �V�[���J�ڃt���O�����Z�b�g
			}
		}

	}
	timer += elapsedTime;
	Graphics::Instance().UpdateConstantBuffer(timer, transTimer);
	GameCleartime += elapsedTime;
	angle += 0.1f;
}

void Game_Clear::Render()
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

	{//�m�C�Y�̉e�����󂯂���̂͂���


	}

	/// �t���[���o�b�t�@�̃f�B�A�N�e�B�x�[�g
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->deactivate(dc);
	// GameOver
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::GameOver)].Get());

	{

		// ��ʃT�C�Y�擾�i���̊֐����B�K�v�ɉ����ĕύX���Ă��������j
		int screenWidth = Graphics::Instance().GetScreenWidth();
		int screenHeight = Graphics::Instance().GetScreenHeight();
		// ��𑜓x�i�Œ�l�j
		constexpr float BASE_WIDTH = 1280.0f;
		constexpr float BASE_HEIGHT = 720.0f;
		// �X�P�[�����O�W���i��������j
		float scaleX = Graphics::Instance().GetWindowScaleFactor().x;
		float scaleY = Graphics::Instance().GetWindowScaleFactor().y;
		// �傫�ȃX�v���C�g�̃T�C�Y
		int bigSize = 512;

		// �����ȃX�v���C�g�̃T�C�Y
		int smallSize = 64;
		int resultsize = smallSize * 8;

		// --- 1. ��ʒ����E�i����Y, �E�[�Ɋ񂹂�j ---
		int resultX = screenWidth / 4 * 3 - bigSize / 2;
		int resultY = screenHeight / 2 - bigSize / 2;
		float loop = 8 * 2;
		for (int i = 0; i < loop; i++)
		{
			if (i % 2 == 0)
			{
				s_bg1->Render(rc, resultX, resultY, 0, bigSize * scaleX, bigSize * scaleY, 0, 0, 512, 512, 360 / loop * i + angle, 1, 1, 1, 0.5f);

			}
			else {

				s_bg1->Render(rc, resultX, resultY, 0, bigSize * scaleX, bigSize * scaleY, 0, 0, 512, 512, -(360 / loop * i + angle), 1, 1, 1, 0.5f);
			}
		}
		s_bg2->Render(rc, resultX, resultY, 0, bigSize * scaleX, bigSize * scaleY, 0, 0, 512, 512, 0, 1, 1, 1, 1);
		s_rank->Render(rc, resultX, resultY, 0, bigSize * scaleX, bigSize * scaleY, 512 * float(result.Result), 0, 512, 512, 0, 1, 1, 1, 1);

		// --- 2. ��ʍ��㒆���i���[�Ɋ񂹂�, �ォ��1/4�̈ʒu�j ---
		int taskX = screenWidth / 10 * 5 - smallSize / 2;
		int taskY = screenHeight / 4 - smallSize / 2;
		s_rank->Render(rc, taskX, taskY, 0, smallSize * scaleX, smallSize * scaleY, 512 * float(result.Task), 0, 512, 512, 0, 1, 1, 1, 1);

		taskX = screenWidth / 10 * 5 - resultsize - smallSize / 2;
		taskY = screenHeight / 4 - smallSize / 2;
		s_result->Render(rc, taskX, taskY, 0, resultsize * scaleX, smallSize * scaleY, 0, 512, 4096, 512, 0, 1, 1, 1, 1);

		// --- 3. ��ʍ��������i���[�Ɋ񂹂�, ������1/4�̈ʒu�j ---
		int deathX = screenWidth / 10 * 5 - smallSize / 2;
		int deathY = screenHeight * 3 / 4 - smallSize / 2;
		s_rank->Render(rc, deathX, deathY, 0, smallSize * scaleX, smallSize * scaleY, 512 * float(result.Death), 0, 512, 512, 0, 1, 1, 1, 1);
		deathX = screenWidth / 10 * 5 - resultsize - smallSize / 2;
		deathY = screenHeight * 3 / 4 - smallSize / 2;

		s_result->Render(rc, deathX, deathY, 0, resultsize * scaleX, smallSize * scaleY, 0, 0, 4096, 512, 0, 1, 1, 1, 1);

	}
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->deactivate(dc);

	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);

	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::Glitch)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->deactivate(dc);

	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::VisionBootDown)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->deactivate(dc);
	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::TemporalNoise)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->deactivate(dc);

	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::NoSignalFinale)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->deactivate(dc);


	Graphics::Instance().bit_block_transfer->blit(
		dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->shader_resource_views[0].GetAddressOf(), 10, 1
	);


}
