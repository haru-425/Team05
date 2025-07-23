#include "SceneClear.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneTitle.h"

#include "System/Audio.h"
#include "System/Input.h"

#include "3DAudio/3DAudio.h"

void Game_Clear::Initialize()
{
	s_rank = new Sprite("Data/Sprite/rank.png");
	s_result = new Sprite("Data/Sprite/result.png");
	s_bg1 = new Sprite("Data/Sprite/rankbg1.png");
	s_bg2 = new Sprite("Data/Sprite/rankbg2.png");
	timer = 0.0f; // タイマー初期化
	transTimer = 0.0f; // シーン遷移タイマー初期化
	sceneTrans = false; // シーン遷移フラグ初期化
	//RankSystem::Instance().SetRank(1, 1, 3);
	result = RankSystem::Instance().GetRank();
	angle = 0;

	// SEの読み込み
	selectSE = Audio::Instance().LoadAudioSource("Data/Sound/selectButton.wav");

	Audio3DSystem::Instance().PlayByTag("electrical_noise");

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
	GameCleartime = 0.0f; ///< デバッグ用タイマー初期化


	timer = 0;
	Audio3DSystem::Instance().StopByTag("electrical_noise"); // 音声停止

	if (selectSE != nullptr)
	{
		selectSE->Stop();
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
			// フラグが立っている間タイマーを加算し、1秒以上経ったらシーン切り替え
			transTimer += elapsedTime;
			if (transTimer >= 3.0f && nextScene != nullptr)
			{
				SceneManager::instance().ChangeScene(new SceneLoading(nextScene));
				nextScene = nullptr; // 多重遷移防止
				sceneTrans = false; // シーン遷移フラグをリセット
			}
		}

	}
	timer += elapsedTime;
	Graphics::Instance().UpdateConstantBuffer(timer, transTimer);
	GameCleartime += elapsedTime;
	angle += 0.1f;
	Audio3DSystem::Instance().UpdateListener(
		{ 0.0f, 0.0f, 0.0f }, // リスナーの位置
		{ 0.0f, 0.0f, 1.0f }, // リスナーの向き
		{ 0.0f, 1.0f, 0.0f }  // リスナーの上方向
	);
	Audio3DSystem::Instance().SetEmitterPositionByTag("electrical_noise", { 0.0f, 0.0f, 0.0f }); // エミッターの位置を更新
	Audio3DSystem::Instance().UpdateEmitters(elapsedTime);

}

void Game_Clear::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();


	/// フレームバッファのクリアとアクティベート（ポストプロセス用）
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->clear(dc, 0, 0, 0, 1);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->activate(dc);

	{//ノイズの影響を受けるものはここ


	}

	/// フレームバッファのディアクティベート
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->deactivate(dc);
	// GameOver
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::GameOver)].Get());
	{
		// 基準解像度（仮想座標系）
		constexpr float BASE_WIDTH = 1280.0f;
		constexpr float BASE_HEIGHT = 720.0f;

		// 実画面サイズ取得
		float screenWidth = static_cast<float>(Graphics::Instance().GetScreenWidth());
		float screenHeight = static_cast<float>(Graphics::Instance().GetScreenHeight());

		// 拡大率（仮想座標→実座標への変換係数）
		float scaleX = screenWidth / BASE_WIDTH;
		float scaleY = screenHeight / BASE_HEIGHT;

		// スプライトサイズ（仮想基準）
		constexpr int BIG_SPRITE_SIZE = 512;
		constexpr int SMALL_SPRITE_SIZE = 64;
		constexpr int NUM_DIGITS = 8;
		// 回転背景（演出）
		int loop = NUM_DIGITS * 2;
		float rotationStep = 360.0f / loop;

		// --- 1. 画面右中央：中央Y, 右寄せ（仮想座標で配置） ---
		float resultCenterX = BASE_WIDTH * 0.75f;
		float resultCenterY = BASE_HEIGHT * 0.5f;

		// 実スケーリング後のサイズ
		float bigSizeX = BIG_SPRITE_SIZE * scaleX;
		float bigSizeY = BIG_SPRITE_SIZE * scaleY;

		// 左上座標に補正（中心を resultCenterX, resultCenterY に合わせる）
		float drawX = resultCenterX * scaleX - bigSizeX * 0.5f;
		float drawY = resultCenterY * scaleY - bigSizeY * 0.5f;

		// 回転背景
		for (int i = 0; i < loop; i++)
		{
			float rotation = rotationStep * i + angle;
			float finalRotation = (i % 2 == 0) ? rotation : -rotation;

			s_bg1->Render(
				rc,
				drawX,
				drawY,
				0,
				bigSizeX,
				bigSizeY,
				0, 0, 512, 512,
				finalRotation,
				1, 1, 1, 0.5f
			);
		}

		// 背景（s_bg2）
		s_bg2->Render(
			rc,
			drawX,
			drawY,
			0,
			bigSizeX,
			bigSizeY,
			0, 0, 512, 512,
			0, 1, 1, 1, 1
		);

		// ランク（s_rank）
		s_rank->Render(
			rc,
			drawX,
			drawY,
			0,
			bigSizeX,
			bigSizeY,
			512.0f * static_cast<float>(result.Result), 0, 512, 512,
			0, 1, 1, 1, 1
		);
		// --- 2. 左上中央：上から1/4位置（仮想位置で配置） ---
		float taskY = BASE_HEIGHT * 0.25f;
		float taskCenterX = BASE_WIDTH * 0.5f;
		float resultLength = SMALL_SPRITE_SIZE * NUM_DIGITS;

		s_rank->Render(rc,
			taskCenterX * scaleX,
			taskY * scaleY,
			0,
			SMALL_SPRITE_SIZE * scaleX,
			SMALL_SPRITE_SIZE * scaleY,
			512.0f * (float)result.Task, 0, 512, 512,
			0, 1, 1, 1, 1
		);

		s_result->Render(rc,
			(taskCenterX - resultLength) * scaleX,
			taskY * scaleY,
			0,
			resultLength * scaleX,
			SMALL_SPRITE_SIZE * scaleY,
			0, 512, 4096, 512,
			0, 1, 1, 1, 1
		);

		// --- 3. 左下中央：下から1/4位置 ---
		float deathY = BASE_HEIGHT * 0.75f;

		s_rank->Render(rc,
			taskCenterX * scaleX,
			deathY * scaleY,
			0,
			SMALL_SPRITE_SIZE * scaleX,
			SMALL_SPRITE_SIZE * scaleY,
			512.0f * (float)result.Death, 0, 512, 512,
			0, 1, 1, 1, 1
		);

		s_result->Render(rc,
			(taskCenterX - resultLength) * scaleX,
			deathY * scaleY,
			0,
			resultLength * scaleX,
			SMALL_SPRITE_SIZE * scaleY,
			0, 0, 4096, 512,
			0, 1, 1, 1, 1
		);
	}
	//{

	//	// 画面サイズ取得（仮の関数名。必要に応じて変更してください）
	//	int screenWidth = Graphics::Instance().GetScreenWidth();
	//	int screenHeight = Graphics::Instance().GetScreenHeight();
	//	// 基準解像度（固定値）
	//	constexpr float BASE_WIDTH = 1280.0f;
	//	constexpr float BASE_HEIGHT = 720.0f;
	//	// スケーリング係数（横幅を基準）
	//	float scaleX = Graphics::Instance().GetWindowScaleFactor().x;
	//	float scaleY = Graphics::Instance().GetWindowScaleFactor().y;
	//	// 大きなスプライトのサイズ
	//	int bigSize = 512;

	//	// 小さなスプライトのサイズ
	//	int smallSize = 64;
	//	int resultsize = smallSize * 8;

	//	// --- 1. 画面中央右（中央Y, 右端に寄せる） ---
	//	int resultX = screenWidth / 4 * 3 - bigSize / 2;
	//	int resultY = screenHeight / 2 - bigSize / 2;
	//	float loop = 8 * 2;
	//	for (int i = 0; i < loop; i++)
	//	{
	//		if (i % 2 == 0)
	//		{
	//			s_bg1->Render(rc, resultX, resultY, 0, bigSize * scaleX, bigSize * scaleY, 0, 0, 512, 512, 360 / loop * i + angle, 1, 1, 1, 0.5f);

	//		}
	//		else {

	//			s_bg1->Render(rc, resultX, resultY, 0, bigSize * scaleX, bigSize * scaleY, 0, 0, 512, 512, -(360 / loop * i + angle), 1, 1, 1, 0.5f);
	//		}
	//	}
	//	s_bg2->Render(rc, resultX, resultY, 0, bigSize * scaleX, bigSize * scaleY, 0, 0, 512, 512, 0, 1, 1, 1, 1);
	//	s_rank->Render(rc, resultX, resultY, 0, bigSize * scaleX, bigSize * scaleY, 512 * float(result.Result), 0, 512, 512, 0, 1, 1, 1, 1);

	//	// --- 2. 画面左上中央（左端に寄せる, 上から1/4の位置） ---
	//	int taskX = screenWidth / 10 * 5 - smallSize / 2;
	//	int taskY = screenHeight / 4 - smallSize / 2;
	//	s_rank->Render(rc, taskX, taskY, 0, smallSize * scaleX, smallSize * scaleY, 512 * float(result.Task), 0, 512, 512, 0, 1, 1, 1, 1);

	//	taskX = screenWidth / 10 * 5 - resultsize - smallSize / 2;
	//	taskY = screenHeight / 4 - smallSize / 2;
	//	s_result->Render(rc, taskX, taskY, 0, resultsize * scaleX, smallSize * scaleY, 0, 512, 4096, 512, 0, 1, 1, 1, 1);

	//	// --- 3. 画面左下中央（左端に寄せる, 下から1/4の位置） ---
	//	int deathX = screenWidth / 10 * 5 - smallSize / 2;
	//	int deathY = screenHeight * 3 / 4 - smallSize / 2;
	//	s_rank->Render(rc, deathX, deathY, 0, smallSize * scaleX, smallSize * scaleY, 512 * float(result.Death), 0, 512, 512, 0, 1, 1, 1, 1);
	//	deathX = screenWidth / 10 * 5 - resultsize - smallSize / 2;
	//	deathY = screenHeight * 3 / 4 - smallSize / 2;

	//	s_result->Render(rc, deathX, deathY, 0, resultsize * scaleX, smallSize * scaleY, 0, 0, 4096, 512, 0, 1, 1, 1, 1);

	//}
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
