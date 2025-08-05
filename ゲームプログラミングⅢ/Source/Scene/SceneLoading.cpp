#include"System/Graphics.h"
#include"System/Input.h"
#include"SceneLoading.h"
#include"SceneManager.h"
#include "Scene/SceneGame.h"

//初期化
void SceneLoading::Initialize()
{
	//スプライト初期化
	sprite = new Sprite("Data/Sprite/LoadingIcon.png");

	//スレッド開始
	thread = new std::thread(LoadingThread, this);
	timer = 0.0f;
	transtimer = 0.0f;
	nextSceneReadyTime = -1.0f; // 次のシーンの準備完了時刻を初期化
	// 3Dオーディオシステムの再生開始
	Audio3DSystem::Instance().PlayByTag("electrical_noise");
	Audio3DSystem::Instance().SetVolumeByAll();
}

//終了化
void SceneLoading::Finalize()
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
	Audio3DSystem::Instance().StopByTag("electrical_noise"); // 音声停止
}

//更新処理
void SceneLoading::Update(float elapsedTime)
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
			if (typeid(*nextScene) == typeid(SceneGame))
			{
				POINT screenPoint = { Graphics::Instance().GetScreenWidth() / 2, Graphics::Instance().GetScreenHeight() / 2 };
				ClientToScreen(Graphics::Instance().GetWindowHandle(), &screenPoint);
				SetCursorPos(screenPoint.x, screenPoint.y);
			}

			SceneManager::instance().ChangeScene(nextScene);
		}
		transtimer += elapsedTime;
	}
	Graphics::Instance().UpdateConstantBuffer(timer, transtimer);
	// 3Dオーディオシステムのエミッター更新
	Audio3DSystem::Instance().UpdateEmitters(elapsedTime);
}

//描画処理
void SceneLoading::Render()
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
		//画面右下にローディングアイコンを描画
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
	//Gritch
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::Glitch)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->deactivate(dc);
	//VHS
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VHS)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VHS)]->activate(dc);

	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::VHS)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VHS)]->deactivate(dc);


	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VHS)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
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
void SceneLoading::DrawGUI()
{

}

//ローディングスレッド
void SceneLoading::LoadingThread(SceneLoading* scene)
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
