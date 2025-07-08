#include"System/Graphics.h"
#include"SceneTitle.h"
#include"System/Input.h"
#include"SceneGame.h"
#include"fujimoto.h"
#include"SceneManager.h"
#include"SceneLoading.h"
#include"SceneGraphics.h"
#include "Scene/SceneMattsu.h"

//初期化
void SceneTitle::Initialize()
{
	//スプライト初期化
	sprite = new Sprite("Data/Sprite/GameTitleStrings.png");
	TitleTimer = 0.0f; // タイトル画面のタイマー初期化
	TitleSignalTimer = 0.0f; // タイトル画面の信号タイマー初期化
	sceneTrans = false; // シーン遷移フラグ初期化
}

//終了化
void SceneTitle::Finalize()
{
	//スプライト終了化
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
}

//更新処理
void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// 任意のゲームパッドボタンが押されているか
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
	// フラグがまだ立っていない場合に入力検出
	if (!sceneTrans)
	{
		if (buttonPressed)
		{
			nextScene = new SceneGame;
			sceneTrans = true;
			TitleSignalTimer = 0.0f; // タイマー再スタート
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
		// フラグが立っている間タイマーを加算し、1秒以上経ったらシーン切り替え
		TitleSignalTimer += elapsedTime;
		if (TitleSignalTimer >= 1.0f && nextScene != nullptr)
		{
			SceneManager::instance().ChangeScene(new SceneLoading(nextScene));
			nextScene = nullptr; // 多重遷移防止
			sceneTrans = false; // シーン遷移フラグをリセット
		}
	}

	TitleTimer += elapsedTime;
	Graphics::Instance().UpdateConstantBuffer(TitleTimer, TitleSignalTimer);
}


//描画処理
void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//描画基準
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();






	/// フレームバッファのクリアとアクティベート（ポストプロセス用）
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->clear(dc, 1, 1, 1, 1);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->activate(dc);

	//タイトルの背景、UIはここで描画
// 2Dスプライト描画
	{
		// タイトル描画
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());

		// スプライトサイズ（実際の描画サイズ）
		float spriteWidth = 2260.0f / 3.0f;
		float spriteHeight = 1077.0f / 3.0f;

		float spritePos[2];
		spritePos[0] = screenWidth / 2.0f - spriteWidth / 2.0f;  // 中央寄せ（X）
		spritePos[1] = 50.0f;  // 上側に固定（Y）

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

	//ポストプロセス適用
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

	/// ポストプロセス結果の描画
	Graphics::Instance().bit_block_transfer->blit(
		dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->shader_resource_views[0].GetAddressOf(), 10, 1
	);
}

//GUI描画
void SceneTitle::DrawGUI()
{

}