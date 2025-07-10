#include"System/Graphics.h"
#include"System/Input.h"
#include"SceneLogo.h"
#include"SceneManager.h"

//初期化
void SceneLogo::Initialize()
{
	//スプライト初期化
	sprite = new Sprite("Data/Sprite/teamlogo.png");

	//スレッド開始
	thread = new std::thread(LoadingThread, this);
	timer = 0.0f;
	transtimer = 0.0f;
	nextSceneReadyTime = -1.0f; // 次のシーンの準備完了時刻を初期化
}

//終了化
void SceneLogo::Finalize()
{
	//スレッド終了化
	if (thread != nullptr)
	{
		thread->join();
		delete thread;
		thread = nullptr;
	}

	//スプライト終了化
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
}

//更新処理
void SceneLogo::Update(float elapsedTime)
{
	constexpr float speed = 180.0f;
	angle += speed * elapsedTime;
	positionX -= 10.0f * elapsedTime;

	// 経過時間を加算
	timer += elapsedTime;

	if (nextScene->IsReady())
	{
		// 準備完了時の時刻を記録（1回だけ）
		if (nextSceneReadyTime < 0.0f)
		{
			nextSceneReadyTime = timer;
		}

		// 準備完了から2秒経過したらシーン遷移
		if ((timer - nextSceneReadyTime) >= 2.0f)
		{
			SceneManager::instance().ChangeScene(nextScene);
		}
		transtimer += elapsedTime;
	}
	Graphics::Instance().UpdateConstantBuffer(timer, transtimer);
}

//描画処理
void SceneLogo::Render()
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



	/// フレームバッファのディアクティベート
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->deactivate(dc);
	// GameOver


	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::GameOver)].Get());


	////2Dスプライト描画
	{
		// 画面サイズを取得
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());

		// スプライトのサイズ（例：512x512ピクセル）
		float spriteWidth = 512.0f;
		float spriteHeight = 512.0f;

		// スプライトを画面中央に配置（左上が中心に来るようにオフセット）
		positionX = (screenWidth - spriteWidth) * 0.5f;
		positionY = (screenHeight - spriteHeight) * 0.5f;

		// スプライトを描画
		sprite->Render(
			rc,                 // 描画コンテキスト
			positionX,          // 中央X位置
			positionY,          // 中央Y位置
			0.0f,               // Z座標
			spriteWidth,        // 幅
			spriteHeight,       // 高さ
			0,              // 回転角
			1.0f, 1.0f, 1.0f, 1.0f // 色（白・不透明）
		);
	}
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->deactivate(dc);
	//Gritch
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::Glitch)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->deactivate(dc);


	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
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

//GUI描画
void SceneLogo::DrawGUI()
{

}

//ローディングスレッド
void SceneLogo::LoadingThread(SceneLogo* scene)
{
	//COM関連の初期化でスレッド毎に呼ぶ必要がある
	CoInitialize(nullptr);

	//次のシーンの初期化を行う
	scene->nextScene->Initialize();

	//スレッドが終わる前にCOM関連の終了化
	CoUninitialize();

	//次のシーンの準備完了設定
	scene->nextScene->SetReady();
}
