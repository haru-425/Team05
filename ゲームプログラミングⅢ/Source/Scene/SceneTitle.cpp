#include"System/Graphics.h"
#include"SceneTitle.h"
#include"System/Input.h"
#include"SceneGame.h"
#include"fujimoto.h"
#include"SceneManager.h"
#include"SceneLoading.h"
#include"SceneGraphics.h"
#include "Scene/SceneMattsu.h"

//������
void SceneTitle::Initialize()
{
	//�X�v���C�g������
	sprite = new Sprite("Data/Sprite/GameTitleStrings.png");
	TitleTimer = 0.0f; // �^�C�g����ʂ̃^�C�}�[������
	TitleSignalTimer = 0.0f; // �^�C�g����ʂ̐M���^�C�}�[������
	sceneTrans = false; // �V�[���J�ڃt���O������
}

//�I����
void SceneTitle::Finalize()
{
	//�X�v���C�g�I����
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
}

//�X�V����
void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// �C�ӂ̃Q�[���p�b�h�{�^����������Ă��邩
	const GamePadButton anyButton =
		GamePad::BTN_A
		| GamePad::BTN_B
		| GamePad::BTN_X
		| GamePad::BTN_Y;

	bool buttonPressed = (anyButton & gamePad.GetButton()) != 0;
	bool fKey = GetAsyncKeyState('F') & 0x8000;
	bool mKey = GetAsyncKeyState('M') & 0x8000;
	bool gKey = GetAsyncKeyState('G') & 0x8000;
	bool pKey = GetAsyncKeyState('P') & 0x8000;
	// �t���O���܂������Ă��Ȃ��ꍇ�ɓ��͌��o
	if (!sceneTrans)
	{
		if (buttonPressed)
		{
			nextScene = new SceneGame;
			sceneTrans = true;
			TitleSignalTimer = 0.0f; // �^�C�}�[�ăX�^�[�g
		}
		else if (fKey)
		{
			nextScene = new fujimoto;
			sceneTrans = true;
			TitleSignalTimer = 0.0f;
		}
		else if (mKey)
		{
			nextScene = new SceneMattsu;
			sceneTrans = true;
			TitleSignalTimer = 0.0f;
		}
		else if (gKey)
		{
			nextScene = new SceneGraphics;
			sceneTrans = true;
			TitleSignalTimer = 0.0f;
		}
		else if (pKey) {
			nextScene = new SceneGame;
			sceneTrans = true;
			TitleSignalTimer = 0.0f;
		}
	}
	else
	{
		// �t���O�������Ă���ԃ^�C�}�[�����Z���A1�b�ȏ�o������V�[���؂�ւ�
		TitleSignalTimer += elapsedTime;
		if (TitleSignalTimer >= 1.0f && nextScene != nullptr)
		{
			SceneManager::instance().ChangeScene(new SceneLoading(nextScene));
			nextScene = nullptr; // ���d�J�ږh�~
			sceneTrans = false; // �V�[���J�ڃt���O�����Z�b�g
		}
	}

	TitleTimer += elapsedTime;
	Graphics::Instance().UpdateConstantBuffer(TitleTimer, TitleSignalTimer);
}


//�`�揈��
void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//�`��
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();






	/// �t���[���o�b�t�@�̃N���A�ƃA�N�e�B�x�[�g�i�|�X�g�v���Z�X�p�j
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->clear(dc, 1, 1, 1, 1);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->activate(dc);

	//�^�C�g���̔w�i�AUI�͂����ŕ`��
// 2D�X�v���C�g�`��
	{
		// �^�C�g���`��
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());

		// �X�v���C�g�T�C�Y�i���ۂ̕`��T�C�Y�j
		float spriteWidth = 2260.0f / 3.0f;
		float spriteHeight = 1077.0f / 3.0f;

		float spritePos[2];
		spritePos[0] = screenWidth / 2.0f - spriteWidth / 2.0f;  // �����񂹁iX�j
		spritePos[1] = 50.0f;  // �㑤�ɌŒ�iY�j

		sprite->Render(
			rc,
			spritePos[0], spritePos[1],
			0,
			spriteWidth, spriteHeight,
			0,
			1, 1, 1, 1
		);
	}


	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->deactivate(dc);

	//�|�X�g�v���Z�X�K�p
//TEMPORAL NOISE
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::TemporalNoise)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->deactivate(dc);
	//Grtch
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::Glitch)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->deactivate(dc);

	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);

	//NoSignalFinale
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::NoSignalFinale)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->deactivate(dc);

	/// �|�X�g�v���Z�X���ʂ̕`��
	Graphics::Instance().bit_block_transfer->blit(
		dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->shader_resource_views[0].GetAddressOf(), 10, 1
	);
}

//GUI�`��
void SceneTitle::DrawGUI()
{

}