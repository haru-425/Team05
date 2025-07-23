#include "SceneGameOver.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneGame.h"
#include "SceneClear.h"
#include "RankSystem/Ranksystem.h"

#include "GameObjects/battery/batteryManager.h"
//int Game_Over::life_number = 2;

void Game_Over::Initialize()
{

	GameOver = new Sprite("Data/Sprite/GameOver.png");
	for (int i = 0; i < 3; i++)
	{
		life[i] = new Life(life_number);
	}

	timer = 0.0f; // �^�C�}�[������
	transTimer = 0.0f; // �V�[���J�ڃ^�C�}�[������

	selectTrans = SelectTrans::Game; // �V�[���J�ڑI��������
	sceneTrans = false; // �V�[���J�ڃt���O������

}

void Game_Over::Finalize()
{
	for (int i = 0; i < 3; i++)
	{
		if (life[i] != nullptr)
		{
			delete life[i];
		}
		life[i] = nullptr;
	}
	if (GameOver != nullptr)
	{
		delete GameOver;
		GameOver = nullptr;
	}
	GameOvertime = 0.0f; ///< �f�o�b�O�p�^�C�}�[������

	timer = 0;

	//�c�@�����炷
	life_number--;

}

void Game_Over::Update(float elapsedTime)
{
	life[life_number]->SetFlag(true);

	for (int i = 0; i < 3; i++)
	{
		life[i]->Update(elapsedTime);
	}

	if (GameOvertime >= 5.0f) {
		if (life_number <= 0 && transTimer <= 0)
		{
			nextScene = new Game_Clear;
			sceneTrans = true;
			transTimer = 0.0f;
			selectTrans = SelectTrans::Game; // �Q�[���I�[�o�[�V�[���ɑJ��

		}
		if (!sceneTrans)
		{

			nextScene = new SceneGame(life_number);
			sceneTrans = true;
			transTimer = 0.0f;
			selectTrans = SelectTrans::Game; // �Q�[���I�[�o�[�V�[���ɑJ��


		}
		else
		{
			// �t���O�������Ă���ԃ^�C�}�[�����Z���A1�b�ȏ�o������V�[���؂�ւ�
			transTimer += elapsedTime;
			if (transTimer >= 3.0f && nextScene != nullptr)
			{
				RankSystem::Instance().SetRank(
					batteryManager::Instance().getPlayerHasBattery(),
					batteryManager::Instance().getMAXBattery(),
					3); // �^�C���A�b�v��S�����N�N
				SceneManager::instance().ChangeScene(new SceneLoading(nextScene));
				nextScene = nullptr; // ���d�J�ږh�~
				sceneTrans = false; // �V�[���J�ڃt���O�����Z�b�g
			}
		}

	}
	timer += elapsedTime;
	Graphics::Instance().UpdateConstantBuffer(timer, transTimer);
	GameOvertime += elapsedTime;
}

void Game_Over::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//�`�揀��
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();
	GameOver->Render(rc, 100, 100, 0, 1095, 316, 0, 1, 1, 1, 1);

	for (int i = 0; i < life_number + 1; i++)
	{
		if (life[i] != nullptr)
		{
			life[i]->Render();
		}
	}
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
		GameOver->Render(rc, 100, 100, 0, 1095, 316, 0, 1, 1, 1, 1);


		//�m�C�Y�̉e�����󂯂Ȃ����̂͂���
		for (int i = 0; i < life_number + 1; i++)
		{
			if (life[i] != nullptr)
			{
				life[i]->Render();
			}
		}

	}
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->deactivate(dc);

	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);

	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::TVNoiseFade)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->deactivate(dc);

	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::NoSignalFinale)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->deactivate(dc);


	Graphics::Instance().bit_block_transfer->blit(
		dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->shader_resource_views[0].GetAddressOf(), 10, 1
	);

	//�V�[���؂�ւ��̓^�C�g������O���t�B�b�N�V�[�����Q�l��
}
