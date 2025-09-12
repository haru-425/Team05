#include "PauseSystem.h"
#include "System/SettingsManager.h"
#include "System/Input.h"
#include "System/Audio.h"

#include <array>
#include <stdexcept>
#include <algorithm>
#include <imgui.h>

#include "Scene/SceneManager.h"
#include "System/Graphics.h"
#include "SceneLoading.h"
#include "SceneGame.h"
#include "SceneTitle.h"

void PauseSystem::Initialize()
{
	/// �X�v���C�g�ǂݍ���
	/// �Q�[���I��
	um.CreateUI("./Data/Sprite/back.png", "SceneGame_Back");
	um.CreateUI("./Data/Sprite/GameTitleStrings.png", "SceneGame_TitleRogo");
	um.CreateUI("./Data/Sprite/pause.png", "SceneGame_Game");
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_Option");
	um.CreateUI("./Data/Sprite/pause.png", "SceneGame_Exit");
	/// �I�v�V��������
	um.CreateUI("./Data/Sprite/back.png", "SceneGame_OptionBack");
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_Sensitivity");
	um.CreateUI("./Data/Sprite/volume.png", "SceneGame_Main");
	um.CreateUI("./Data/Sprite/volume.png", "SceneGame_BGM");
	um.CreateUI("./Data/Sprite/volume.png", "SceneGame_SE");
	/// ���x
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_OptionBarBack");
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_OptionBar"); ///< 11
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_100");
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_10");
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_1");
	/// �}�X�^�[�p�o�[
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_MainBarBack");
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_MainBar"); ///< 16
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_Main100");
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_Main10");
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_Main1");
	/// BGM�p�o�[
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_BGMBarBack");
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_BGMBar"); ///< 21
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_BGM100");
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_BGM10");
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_BGM1");
	/// SE�p�o�[
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_SEBarBack");
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_SEBar"); ///< 26
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_SE100");
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_SE10");
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_SE1");

	isVolumeSliderActive = false;
	oldSelect = -1;

	for (auto& p : um.GetUIs()) {
		p->GetSpriteData().isVisible = true;
	}

	GameSettings setting = SettingsManager::Instance().GetGameSettings();
	sensitivity = setting.sensitivity * 100; ///< ���x
	mVolume = setting.masterVolume * 100; ///< �}�X�^�[
	bgmVolume = setting.bgmVolume * 100; ///< BGM
	seVolume = setting.seVolume * 100; ///< SE

	/// �X���C�_�[�̈ʒu�����킹��                     �o�[�̒[����o�[�̒�����ݒ�l�Ŋ������Ƃ��낪�X���C�_�[�̈ʒu
	um.GetUIs().at(11)->GetSpriteData().spritePos.x = BAR_MIN + (BAR_WIDTH * setting.sensitivity) - SLIDER_WIDTH;
	um.GetUIs().at(16)->GetSpriteData().spritePos.x = BAR_MIN + (BAR_WIDTH * setting.masterVolume) - SLIDER_WIDTH;
	um.GetUIs().at(21)->GetSpriteData().spritePos.x = BAR_MIN + (BAR_WIDTH * setting.bgmVolume) - SLIDER_WIDTH;
	um.GetUIs().at(26)->GetSpriteData().spritePos.x = BAR_MIN + (BAR_WIDTH * setting.seVolume) - SLIDER_WIDTH;

	/// SE�ǂݍ���
	selectSE = Audio::Instance().LoadAudioSource("Data/Sound/selectButton.wav");
}

void PauseSystem::Finalize()
{
	um.Clear();

	// SE�̏I����
	if (selectSE != nullptr)
	{
		selectSE->Stop();
		delete selectSE;
		selectSE = nullptr;
	}
}

void PauseSystem::Update(float elapsedTime)
{
	Input::Instance().GetGamePad().strength = { 0,0 };

	/// �Q�[���p�b�h���悤�̏ꍇ
	if (Input::Instance().GetIsGamePadActive())
	{
		UpdateWithController(elapsedTime);
		return;
	}
	selectNum = -1;

	GameSettings setting = SettingsManager::Instance().GetGameSettings();
	selectSE->SetVolume(0.5f * setting.seVolume * setting.masterVolume);

	static bool selectOptions = true;
	static bool selectStart = false;
	static bool previousDow = false;
	static int lastSelectID = -1;
	/// �ŏ��V�[����ǂݍ��񂾂Ƃ������������ʂ�
	/// UI��ݒ�ɍ��킹�邽�߂ɐݒ�̒l��\���Ɏg���ϐ��ɑ��

	//// option��I��������Ԃ���n�߂�
	selectOptions = true;
	selectStart = false;

	Mouse& mouse = Input::Instance().GetMouse();
	SceneManager& sm = SceneManager::instance();

	/// �}�E�X���W�擾
	DirectX::XMFLOAT2 scaleFactor = Graphics::Instance().GetWindowScaleFactor();
	DirectX::XMFLOAT2 mousePos = { (float)Input::Instance().GetMouse().GetPositionX(), (float)Input::Instance().GetMouse().GetPositionY() };
	um.Update(mousePos);


	for (auto& ui : um.GetUIs())
	{
		int id = ui->GetID();

		// �Ώ�ID�̂ݏ���
		if (!(id == 2 || id == 3 || id == 4 ||
			id == 11 || id == 16 || id == 21 || id == 26)) continue;

	    /// ��ɕ`�悳�ꂢ�Ă���UI��\��
		if (id < 5) {
			ui->GetSpriteData().isVisible = true;
		}

		bool isHit = ui->GetIsHit();
		bool isSelected = (id == lastSelectID);
		bool isSpecialID = (id == 11 || id == 16 || id ==21 || id == 26);

		bool shouldHighlight = false;

		// �I�𒆂̂Ƃ��� isSpecialID �̒��őI�����ꂽUI�������n�C���C�g
		if (lastSelectID >= 0) {
			if (isSpecialID) {
				shouldHighlight = isSelected;
			}
			else {
				shouldHighlight = (id == 3 && selectOptions) || (id == 2 && selectStart);
			}
		}
		else {
			// �I������Ă��Ȃ��ꍇ�̒ʏ폈��
			if (isSpecialID) {
				if (!isVolumeSliderActive) {
					shouldHighlight = isHit;
				}
			}
			else {
				shouldHighlight = isHit || (id == 3 && selectOptions) || (id == 2 && selectStart);
			}
		}

		ui->GetSpriteData().color = shouldHighlight ?
			ui->GetSpriteData().color = { 1, 1, 1, 1 } : ui->GetSpriteData().color = { 0.660f, 0.660f, 0.660f, 1 };
	}

	if (!(mouse.GetButton() & mouse.BTN_LEFT))
	{
		previousDow = false;
		lastSelectID = -1;
	}

	if (isCenterCursor)
	{
		isSceneStart = false;
		SceneGame::SetPause(false);
		isCenterCursor = false;
		if (oldSelect != 0 || selectSE->IsPlaying()) {
			selectSE->Play(false);
			oldSelect = 0;
		}
		return;
	}

	for (auto& ui : um.GetHitAllUI())
	{
		int id = ui->GetID();

		switch (id)
		{
		case 2: ///< id 2�̓Q�[���J�n

			if (mouse.GetButtonDown() & mouse.BTN_LEFT)
			{
				selectStart = true;

				if (selectOptions)
					selectOptions = !selectOptions;


				if (!isCenterCursor)
				{
					POINT screenPoint = { Input::Instance().GetMouse().GetScreenWidth() / 2, Input::Instance().GetMouse().GetScreenHeight() / 2 };
					ClientToScreen(Graphics::Instance().GetWindowHandle(), &screenPoint);
					SetCursorPos(screenPoint.x, screenPoint.y);
					isCenterCursor = true;
				}

				break;
			}
			break;
		case 3: ///< id 3�͐ݒ�
			if (mouse.GetButtonDown() & mouse.BTN_LEFT)
			{
				selectOptions = true;

				if (selectStart)
					selectStart = !selectStart;
				if (oldSelect != 1 || selectSE->IsPlaying()) {
					oldSelect = 1;
					selectSE->Play(false);
				}

				break;
			}
			break;
		case 4: /// �^�C�g���ɑJ��
			if (mouse.GetButtonDown() & mouse.BTN_LEFT)
			{
				SceneGame::SetPause(false);
				SceneManager::instance().ChangeScene(new SceneLoading(new SceneTitle));
				isSceneStart = false;
				break;
			}
			break;
		case 11:
		case 16:
		case 21:
		case 26:
			if ((mouse.GetButtonDown() & mouse.BTN_LEFT) && um.GetUIs().at(id)->GetIsHit()) {
				um.GetUIs().at(id)->GetSpriteData().spriteSize = { 20, 58 };
				isVolumeSliderActive = true;
				lastSelectID = id;
			}

			if (isVolumeSliderActive) {
				if (mouse.GetButton() & mouse.BTN_LEFT || previousDow) {
					previousDow = true;
				}
				else if (mouse.GetButtonUp() & mouse.BTN_LEFT || previousDow) {
					isVolumeSliderActive = false;
				}
			}
			break;

		default:
			if (!previousDow)
			{
				um.GetUIs().at(11)->GetSpriteData().spriteSize = { 16,54 };
				um.GetUIs().at(16)->GetSpriteData().spriteSize = { 16,54 };
				um.GetUIs().at(21)->GetSpriteData().spriteSize = { 16,54 };
				um.GetUIs().at(26)->GetSpriteData().spriteSize = { 16,54 };
			}

			break;
		}
	}
	if (previousDow) {
		int x = 0;
	}

	/// ���x�Ƃ��̃o�[�̓���
	if (previousDow)
	{
		if (lastSelectID == 11)
		{
			SetVolume(lastSelectID, lastSelectID - 1, mousePos, sensitivity, scaleFactor);
		}
		else if (lastSelectID == 16)
		{
			SetVolume(lastSelectID, lastSelectID - 1, mousePos, mVolume, scaleFactor);
		}
		else if (lastSelectID == 21)
		{
			SetVolume(lastSelectID, lastSelectID - 1, mousePos, bgmVolume, scaleFactor);
		}
		else if (lastSelectID == 26)
		{
			SetVolume(lastSelectID, lastSelectID - 1, mousePos, seVolume, scaleFactor);
		}
	}

	/// �e�N�X�`���̈ʒu��ݒ�
	SetVolumeTexturePosition(12, sensitivity, 25);
	SetVolumeTexturePosition(17, mVolume, 25);
	SetVolumeTexturePosition(22, bgmVolume, 25);
	SetVolumeTexturePosition(27, seVolume, 25);

	/// �ݒ��ύX�����ꍇ�ۑ�
	{
		setting.sensitivity = sensitivity * 0.01;
		setting.masterVolume = mVolume * 0.01;
		setting.bgmVolume = bgmVolume * 0.01;
		setting.seVolume = seVolume * 0.01;

		SettingsManager::Instance().SetGameSettings(setting);
	}
	Audio3DSystem::Instance().masterVolume = setting.masterVolume;
	Audio3DSystem::Instance().seVolume = setting.seVolume;
	Audio3DSystem::Instance().bgmVolume = setting.bgmVolume;
	Audio3DSystem::Instance().SetVolumeByAll();
}

void PauseSystem::Render(RenderContext& rc)
{
	/// UI�`��
	um.Render(rc);
}

void PauseSystem::DrawDebug()
{
	um.DrawDebug();

	if(ImGui::TreeNode("option"))
	{
		ImGui::InputInt("sensivity", &sensitivity);
		ImGui::InputInt("mVolume", &mVolume);
		ImGui::InputInt("bgmVolume", &bgmVolume);
		ImGui::InputInt("seVolume", &seVolume);
		ImGui::InputInt("selectNum", &selectNum);

		ImGui::TreePop();
	}
}


void PauseSystem::SetVolume(int lastSelectID, int barID, DirectX::XMFLOAT2 mousePos, int& volume, DirectX::XMFLOAT2 scaleFactor)
{
	///
	///                             ��UI�̕`��n�_
	///                             /--/ <-�X���C�_�[
	///                              ���}�E�X�̍��W = BAR_MIN + SLIDER_WIDTH / 2
	///                             /---------------------------------------------------------------/
	///

	mousePos.x = std::clamp(((float)mousePos.x / scaleFactor.x), (BAR_MIN + SLIDER_WIDTH / 2), (BAR_MAX - SLIDER_WIDTH / 2));
	volume = um.GetUIs().at(lastSelectID)->GetSpriteData().spritePos.x = (mousePos.x) - (SLIDER_WIDTH / 2);
	volume -= um.GetUIs().at(barID)->GetSpriteData().spritePos.x; ///< �Q�[�W��UI��X���W
	float barWidth = (BAR_MAX - BAR_MIN) - SLIDER_WIDTH;
	volume /= barWidth / 100;
}

void PauseSystem::SetVolumeTexturePosition(int startID, int volume, float texWight)
{
	int digits[3]{};

	digits[0] = volume / 100 % 10;  /// 100�̈�
	digits[1] = volume / 10 % 10;	/// 10�̈�
	digits[2] = volume % 10;        /// 1�̈� 

	for (int i = 0; i < 3; ++i) {
		/// 0�̎������e�N�X�`���̊�ʒu����ԉE��
		if (digits[i] == 0) { 
			um.GetUIs().at(startID + i)->GetSpriteData().texturePos.x = 30 + texWight * 9; 
			continue;
		}
		um.GetUIs().at(startID + i)->GetSpriteData().texturePos.x = 30 + texWight * (digits[i] - 1);
	}

	for (int i = 0; i < 3; ++i) {
		/// �\���ݒ�
		um.GetUIs().at(startID + i)->GetSpriteData().isVisible = true;
		/// �F�ݒ�
		um.GetUIs().at(startID + i)->GetSpriteData().color = {1,1,1,1};
	}
	/// 100�����̎���100�̈ʂ��\���ɂ���
	if (volume < 100) { um.GetUIs().at(startID)->GetSpriteData().isVisible = false; }
	/// 10�����̎���10�̈ʂ��\���ɂ���
	if (volume < 10) { um.GetUIs().at(startID + 1)->GetSpriteData().isVisible = false; }

}

void _BarMove(int id, const DirectX::XMFLOAT3& barInfo, int settingValue, UIManager* um)
{
	float valueParHundred = (barInfo.y - barInfo.z / 2) - (barInfo.x + barInfo.z / 2);
	valueParHundred /= 100;

	um->GetUIs().at(id)->GetSpriteData().spritePos.x = barInfo.x + valueParHundred * settingValue;
}

/**
* �R���g���[���[�p�̃|�[�Y�@�\
*/
void PauseSystem::UpdateWithController(float elapsedTime)
{
	auto& gamePad = Input::Instance().GetGamePad();
	GameSettings setting = SettingsManager::Instance().GetGameSettings();

	static bool selectOption = false;

	/// �V�[���͂��܂��ď��߂̏���
	if (!isSceneStart)
	{
		selectOption = false;
		selectNum = 3;
		isSceneStart = true;
	}

	/// �n�C���C�g����
	for (auto& ui : um.GetUIs())
	{
		int id = ui->GetID();

		// �Ώ�ID�̂ݏ���
		if (!(id == 2 || id == 3 || id == 4 ||
			id == 11 || id == 16 || id == 21 || id == 26)) continue;

		/// ��ɕ`�悳�ꂢ�Ă���UI��\��
		if (id < 5) {
			ui->GetSpriteData().isVisible = true;
		}

		ui->GetSpriteData().color = ui->GetIsHit() ?
			ui->GetSpriteData().color = { 1, 1, 1, 1 } : ui->GetSpriteData().color = { 0.660f, 0.660f, 0.660f, 1 };
	}

	/// �R���g���[���[�ł̍��ڑI��
	if (selectOption)
	{
		if (gamePad.GetButtonDown() & GamePad::BTN_DOWN)  selectNum += 5;
		if (gamePad.GetButtonDown() & GamePad::BTN_UP) selectNum -= 5;

		if (selectNum > 26)selectNum = 11;
		if (selectNum < 11)selectNum = 26;
	}
	else
	{
		if (gamePad.GetButtonDown() & GamePad::BTN_DOWN)  selectNum++;
		if (gamePad.GetButtonDown() & GamePad::BTN_UP) selectNum--;

		if (selectNum > 4)selectNum = 2;
		if (selectNum < 2)selectNum = 4;
	}

	if(selectNum > 0)
		um.GetUIs().at(selectNum)->SetIsHit(true);

	bool isMoveBarR = false;
	bool isMoveBarL = false;

	/// �S�̂̒���
	float valueParHun = (BAR_MAX - SLIDER_WIDTH / 2) - (BAR_MIN + SLIDER_WIDTH / 2);
	/// 1�łǂꂾ���i�ނ̂�
	valueParHun /= 100;
	/// �q�b�g UI ����
	for (auto& ui : um.GetHitAllUI())
	{
		int id = ui->GetID();

		switch (id)
		{
		case 2: ///< �Q�[���J�n
			if (gamePad.GetButtonDown() & GamePad::BTN_A)
			{
				selectSE->Play(false);
				SceneGame::SetPause(false);
				isSceneStart = false;
			}

			um.GetUIs().at(3)->SetIsHit(false);
			um.GetUIs().at(4)->SetIsHit(false);

			break;
		case 3: ///< �ݒ�
			if (gamePad.GetButtonDown() & GamePad::BTN_A)
			{
				selectOption = true;
				selectSE->Play(false);
				selectNum = 11;
			}

			um.GetUIs().at(2)->SetIsHit(false);
			um.GetUIs().at(4)->SetIsHit(false);

			break;
		case 4: ///< �I��
			if (gamePad.GetButtonDown() & GamePad::BTN_A)
			{
				SceneGame::SetPause(false);
				SceneManager::instance().ChangeScene(new SceneLoading(new SceneTitle));
				selectSE->Play(false);
				isSceneStart = false;
			}

			um.GetUIs().at(2)->SetIsHit(false);
			um.GetUIs().at(3)->SetIsHit(false);

			break;
		case 11:
			[[fallthrough]];
		case 16:
			[[fallthrough]];
		case 21:
			[[fallthrough]];
		case 26:
			isVolumeSliderActive = true;

			if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_LEFT || Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT)
			{
				selectTime -= elapsedTime;
				if (selectTime <= 0)
				{
					if (gamePad.GetButton() & GamePad::BTN_RIGHT)isMoveBarR = true;
					if (gamePad.GetButton() & GamePad::BTN_LEFT)isMoveBarL = true;

					++selectCount;

					switch (selectCount)
					{
					case 1:
						selectTime = 0.3f;
						break;
					case 2:
						selectTime = 0.3f;
						break;
					case 3:
						selectTime = 0.2f;
						break;
					case 4:
						selectTime = 0.2f;
						break;
					case 5:
						selectTime = 0.1f;
						break;
					case 6:
						selectTime = 0.1f;
						break;
					default:
						selectTime = 0.03;
						break;
					}
				}
			}
			else
			{
				selectTime = 0;
				selectCount = 0;
			}

			/// ���삷��o�[�ȊO�̃q�b�g�����S�Ď�����
			for (int selectId = 11; selectId < 27; selectId += 5)
			{
				if (selectId == selectNum)continue;

				um.GetUIs().at(selectId)->SetIsHit(false);
			}


			switch (selectNum)
			{
			case 11:
				if (isMoveBarR)
					sensitivity++;
				if (isMoveBarL)
					sensitivity--;

				um.GetUIs().at(selectNum)->GetSpriteData().spritePos.x = BAR_MIN + valueParHun * sensitivity;
				break;
			case 16:
				if (isMoveBarR)
					mVolume++;
				if (isMoveBarL)
					mVolume--;

				um.GetUIs().at(selectNum)->GetSpriteData().spritePos.x = BAR_MIN + valueParHun * mVolume;
				break;
			case 21:
				if (isMoveBarR)
					bgmVolume++;
				if (isMoveBarL)
					bgmVolume--;

				um.GetUIs().at(selectNum)->GetSpriteData().spritePos.x = BAR_MIN + valueParHun * bgmVolume;
				break;
			case 26:
				if (isMoveBarR)
					seVolume++;
				if (isMoveBarL)
					seVolume--;

				um.GetUIs().at(selectNum)->GetSpriteData().spritePos.x = BAR_MIN + valueParHun * seVolume;
				break;
			default:
				break;
			}
			sensitivity = std::clamp(sensitivity, 0, 100);
			mVolume		= std::clamp(mVolume,	  0, 100);
			bgmVolume	= std::clamp(bgmVolume,	  0, 100);
			seVolume	= std::clamp(seVolume,	  0, 100);

			/// B�{�^�����������Ƃ��ɑS���̃q�b�g�������x�������A
			/// ����Ώۂ��ĊJ�A�ݒ�A�I���ɖ߂��ƂƂ��Ƀq�b�g�����ݒ�ɂ���
			if (gamePad.GetButton() & GamePad::BTN_B)
			{
				selectNum = 3;
				for (int selectId = 11; selectId < 27; selectId += 5)
				{
					um.GetUIs().at(selectId)->SetIsHit(false);
				}
				selectOption = false;
			}
			break;
		default:
			break;
		}
	}

	/// �e�N�X�`���̈ʒu��ݒ�
	SetVolumeTexturePosition(12, sensitivity, 25);
	SetVolumeTexturePosition(17, mVolume, 25);
	SetVolumeTexturePosition(22, bgmVolume, 25);
	SetVolumeTexturePosition(27, seVolume, 25);

	/// �ݒ��ύX�����ꍇ�ۑ�
	{
		setting.sensitivity = sensitivity * 0.01;
		setting.masterVolume = mVolume * 0.01;
		setting.bgmVolume = bgmVolume * 0.01;
		setting.seVolume = seVolume * 0.01;

		SettingsManager::Instance().SetGameSettings(setting);
	}
	Audio3DSystem::Instance().masterVolume = setting.masterVolume;
	Audio3DSystem::Instance().seVolume = setting.seVolume;
	Audio3DSystem::Instance().bgmVolume = setting.bgmVolume;
	Audio3DSystem::Instance().SetVolumeByAll();
}
