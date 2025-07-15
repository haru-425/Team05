#include "SceneClear.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneTitle.h"
void Game_Clear::Initialize()
{
	GameOver = new Sprite("Data/Sprite/GameOver.png");
	timer = 0.0f; // タイマー初期化
	transTimer = 0.0f; // シーン遷移タイマー初期化
	sceneTrans = false; // シーン遷移フラグ初期化
}

void Game_Clear::Finalize()
{

	if (GameOver != nullptr)
	{
		delete GameOver;
		GameOver = nullptr;
	}
	GameCleartime = 0.0f; ///< デバッグ用タイマー初期化


	timer = 0;

}

void Game_Clear::Update(float elapsedTime)
{


	if (GameCleartime >= 5.0f) {
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
	GameOver->Render(rc, 100, 100, 0, 1095, 316, 0, 1, 1, 1, 1);


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
		GameOver->Render(rc, 100, 100, 0, 1095, 316, 0, 1, 1, 1, 1);


	}
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->deactivate(dc);

	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);

	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::VisionBootDown)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->deactivate(dc);

	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::NoSignalFinale)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->deactivate(dc);


	Graphics::Instance().bit_block_transfer->blit(
		dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->shader_resource_views[0].GetAddressOf(), 10, 1
	);

	//シーン切り替えはタイトルからグラフィックシーンを参考に
}
