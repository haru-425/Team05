#include "SceneGameOver.h"


void Game_Over::Initialize()
{
	GameOver = new Sprite("Data/Sprite/GameOver.png");
	for (int i = 0; i < 3; i++)
	{
		life[i] = new Life();
	}
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
}

void Game_Over::Update(float elapsedTime)
{

	Graphics::Instance().UpdateConstantBuffer(GameOvertime);
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

	for (int i = 0; i < 3; i++)
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
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::GameOver)].Get());

	{
		//�m�C�Y�̉e�����󂯂Ȃ����̂͂���

	}
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->deactivate(dc);


	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);


	Graphics::Instance().bit_block_transfer->blit(
		dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1
	);

	//�V�[���؂�ւ��̓^�C�g������O���t�B�b�N�V�[�����Q�l��
}
