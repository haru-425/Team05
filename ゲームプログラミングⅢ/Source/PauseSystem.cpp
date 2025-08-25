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
	/// スプライト読み込み
	/// ゲーム選択
	um.CreateUI("./Data/Sprite/back.png", "SceneGame_Back");
	um.CreateUI("./Data/Sprite/GameTitleStrings.png", "SceneGame_TitleRogo");
	um.CreateUI("./Data/Sprite/pause.png", "SceneGame_Game");
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_Option");
	um.CreateUI("./Data/Sprite/pause.png", "SceneGame_Exit");
	/// オプション項目
	um.CreateUI("./Data/Sprite/back.png", "SceneGame_OptionBack");
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_Sensitivity");
	um.CreateUI("./Data/Sprite/volume.png", "SceneGame_Main");
	um.CreateUI("./Data/Sprite/volume.png", "SceneGame_BGM");
	um.CreateUI("./Data/Sprite/volume.png", "SceneGame_SE");
	/// 感度
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_OptionBarBack");
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_OptionBar"); ///< 11
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_100");
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_10");
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_1");
	/// マスター用バー
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_MainBarBack");
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_MainBar"); ///< 16
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_Main100");
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_Main10");
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_Main1");
	/// BGM用バー
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_BGMBarBack");
	um.CreateUI("./Data/Sprite/image.png", "SceneGame_BGMBar"); ///< 21
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_BGM100");
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_BGM10");
	um.CreateUI("./Data/Sprite/numbers.png", "SceneGame_BGM1");
	/// SE用バー
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
	sensitivity = setting.sensitivity * 100; ///< 感度
	mVolume = setting.masterVolume * 100; ///< マスター
	bgmVolume = setting.bgmVolume * 100; ///< BGM
	seVolume = setting.seVolume * 100; ///< SE

	/// スライダーの位置を合わせる                     バーの端からバーの長さを設定値で割ったところがスライダーの位置
	um.GetUIs().at(11)->GetSpriteData().spritePos.x = BAR_MIN + (BAR_WIDTH * setting.sensitivity) - SLIDER_WIDTH;
	um.GetUIs().at(16)->GetSpriteData().spritePos.x = BAR_MIN + (BAR_WIDTH * setting.masterVolume) - SLIDER_WIDTH;
	um.GetUIs().at(21)->GetSpriteData().spritePos.x = BAR_MIN + (BAR_WIDTH * setting.bgmVolume) - SLIDER_WIDTH;
	um.GetUIs().at(26)->GetSpriteData().spritePos.x = BAR_MIN + (BAR_WIDTH * setting.seVolume) - SLIDER_WIDTH;

	/// SE読み込み
	selectSE = Audio::Instance().LoadAudioSource("Data/Sound/selectButton.wav");
}

void PauseSystem::Finalize()
{
	um.Clear();

	// SEの終了化
	if (selectSE != nullptr)
	{
		selectSE->Stop();
		delete selectSE;
		selectSE = nullptr;
	}
}

void PauseSystem::Update(float elapsedTime)
{
	GameSettings setting = SettingsManager::Instance().GetGameSettings();
	selectSE->SetVolume(0.5f * setting.seVolume * setting.masterVolume);

	static bool selectOptions = true;
	static bool selectStart = false;
	static bool previousDow = false;
	static int lastSelectID = -1;
	/// 最初シーンを読み込んだときだけ処理が通り
	/// UIを設定に合わせるために設定の値を表示に使う変数に代入

	//// optionを選択した状態から始める
	selectOptions = true;
	selectStart = false;

	Mouse& mouse = Input::Instance().GetMouse();
	SceneManager& sm = SceneManager::instance();

	/// マウス座標取得
	DirectX::XMFLOAT2 scaleFactor = Graphics::Instance().GetWindowScaleFactor();
	DirectX::XMFLOAT2 mousePos = { (float)Input::Instance().GetMouse().GetPositionX(), (float)Input::Instance().GetMouse().GetPositionY() };
	um.Update(mousePos);


	for (auto& ui : um.GetUIs())
	{
		int id = ui->GetID();

		// 対象IDのみ処理
		if (!(id == 2 || id == 3 || id == 4 ||
			id == 11 || id == 16 || id == 21 || id == 26)) continue;

	    /// 常に描画されいているUIを表示
		if (id < 5) {
			ui->GetSpriteData().isVisible = true;
		}

		bool isHit = ui->GetIsHit();
		bool isSelected = (id == lastSelectID);
		bool isSpecialID = (id == 11 || id == 16 || id ==21 || id == 26);

		bool shouldHighlight = false;

		// 選択中のときは isSpecialID の中で選択されたUIだけをハイライト
		if (lastSelectID >= 0) {
			if (isSpecialID) {
				shouldHighlight = isSelected;
			}
			else {
				shouldHighlight = (id == 3 && selectOptions) || (id == 2 && selectStart);
			}
		}
		else {
			// 選択されていない場合の通常処理
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

	for (auto& ui : um.GetHitAllUI())
	{
		int id = ui->GetID();

		switch (id)
		{
		case 2: ///< id 2はゲーム開始
			if (mouse.GetButtonDown() & mouse.BTN_LEFT)
			{
				selectStart = true;

				if (selectOptions)
					selectOptions = !selectOptions;
				if (oldSelect != 0 || selectSE->IsPlaying()) {
					selectSE->Play(false);
					oldSelect = 0;
				}

				SceneGame::SetPause(false);
				break;
			}
			break;
		case 3: ///< id 3は設定
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
		case 4: /// タイトルに遷移
			if (mouse.GetButtonDown() & mouse.BTN_LEFT)
			{
				SceneGame::SetPause(false);
				SceneManager::instance().ChangeScene(new SceneLoading(new SceneTitle));
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

	/// 感度とかのバーの動作
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

	/// テクスチャの位置を設定
	SetVolumeTexturePosition(12, sensitivity, 25);
	SetVolumeTexturePosition(17, mVolume, 25);
	SetVolumeTexturePosition(22, bgmVolume, 25);
	SetVolumeTexturePosition(27, seVolume, 25);

	/// 設定を変更した場合保存
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
	/// UI描画
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

		ImGui::TreePop();
	}
}


void PauseSystem::SetVolume(int lastSelectID, int barID, DirectX::XMFLOAT2 mousePos, int& volume, DirectX::XMFLOAT2 scaleFactor)
{
	///
	///                             ↓UIの描画始点
	///                             /--/ <-スライダー
	///                              ↑マウスの座標 = BAR_MIN + SLIDER_WIDTH / 2
	///                             /---------------------------------------------------------------/
	///

	mousePos.x = std::clamp(((float)mousePos.x / scaleFactor.x), (BAR_MIN + SLIDER_WIDTH / 2), (BAR_MAX - SLIDER_WIDTH / 2));
	volume = um.GetUIs().at(lastSelectID)->GetSpriteData().spritePos.x = (mousePos.x) - (SLIDER_WIDTH / 2);
	volume -= um.GetUIs().at(barID)->GetSpriteData().spritePos.x; ///< ゲージのUIのX座標
	float barWidth = (BAR_MAX - BAR_MIN) - SLIDER_WIDTH;
	volume /= barWidth / 100;
}

void PauseSystem::SetVolumeTexturePosition(int startID, int volume, float texWight)
{
	int digits[3]{};

	digits[0] = volume / 100 % 10;  /// 100の位
	digits[1] = volume / 10 % 10;	/// 10の位
	digits[2] = volume % 10;        /// 1の位 

	for (int i = 0; i < 3; ++i) {
		/// 0の時だけテクスチャの基準位置を一番右に
		if (digits[i] == 0) { 
			um.GetUIs().at(startID + i)->GetSpriteData().texturePos.x = 30 + texWight * 9; 
			continue;
		}
		um.GetUIs().at(startID + i)->GetSpriteData().texturePos.x = 30 + texWight * (digits[i] - 1);
	}

	for (int i = 0; i < 3; ++i) {
		/// 表示設定
		um.GetUIs().at(startID + i)->GetSpriteData().isVisible = true;
		/// 色設定
		um.GetUIs().at(startID + i)->GetSpriteData().color = {1,1,1,1};
	}
	/// 100未満の時に100の位を非表示にする
	if (volume < 100) { um.GetUIs().at(startID)->GetSpriteData().isVisible = false; }
	/// 10未満の時に10の位を非表示にする
	if (volume < 10) { um.GetUIs().at(startID + 1)->GetSpriteData().isVisible = false; }

}