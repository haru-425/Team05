#include "System/Graphics.h"
#include "SceneGame.h"
#include "Camera/Camera.h"
#include "System/GamePad.h"
#include "System/Input.h"
#include"Scene/SceneGameOver.h"
#include"Scene/SceneManager.h"

// 初期化
void SceneGame::Initialize()
{
	//ステージ初期化
	stage = new Stage();

	//カメラ初期設定
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
		);

	//カメラコントローラー初期化
	i_CameraController = std::make_unique<FPCameraController>();

	player = std::make_shared<Player>();

	//ミニマップスプライト初期化
	minimap = new MiniMap();
}

// 終了化
void SceneGame::Finalize()
{
	//ステージ終了化
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}
	//ミニマップ終了化
	if (minimap != nullptr)
	{
		delete minimap;
		minimap = nullptr;
	}
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	GamePad& gamepad = Input::Instance().GetGamePad();

	//ゲームオーバーに強制遷移
	if (GetAsyncKeyState('Z') & 0x8000)
	{
		// Zキーが押されているときに実行される
		SceneManager::instance().ChangeScene(new Game_Over);
	}


	//ステージ更新処理
	stage->Update(elapsedTime);
	player->Update(elapsedTime);
	minimap->Update(player->GetPosition());

	// 一人称用カメラ
	if (typeid(*i_CameraController) == typeid(FPCameraController))
	{
		POINT screenPoint = { Input::Instance().GetMouse().GetScreenWidth() / 2, Input::Instance().GetMouse().GetScreenHeight() / 2 };
		ClientToScreen(Graphics::Instance().GetWindowHandle(), &screenPoint);
		DirectX::XMFLOAT3 cameraPos = player->GetPosition();
		i_CameraController->SetCameraPos(cameraPos);
		i_CameraController->Update(elapsedTime);
		SetCursorPos(screenPoint.x, screenPoint.y);

		if (gamepad.GetButton() & GamePad::CTRL && gamepad.GetButtonDown() & GamePad::BTN_X)
		{
			i_CameraController = std::make_unique<FreeCameraController>();
		}
	}
	// フリーカメラ
	else
	{
		i_CameraController->Update(elapsedTime);

		if (gamepad.GetButton() & GamePad::CTRL && gamepad.GetButtonDown() & GamePad::BTN_X)
		{
			i_CameraController = std::make_unique<FPCameraController>();
		}
	}
}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();

	Camera::Instance().SetPerspectiveFov(45,
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f);


	// 描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	
rc.lightDirection = { 0.0f, -1.0f, 0.0f };	// ライト方向（下方向）
	rc.renderState = graphics.GetRenderState();

	//カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->clear(dc, 0.5f, 0.5f, 1, 1);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->activate(dc);
	// 3Dモデル描画
	{
		//ステージ描画
		stage->Render(rc, modelRenderer);

		player->Render(rc, modelRenderer);
	}

	// 3Dデバッグ描画
	{
		player->RenderDebug(rc, shapeRenderer, { 1,2,1 }, { 1,1,1,1 }, DEBUG_MODE::BOX | DEBUG_MODE::CAPSULE);
	}
	
	// 2Dスプライト描画
	{
		//minimap->Render(player->GetPosition());
	}
	/// フレームバッファのディアクティベート
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->deactivate(dc);

	// BLOOM
	Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::BloomFinal]->clear(dc);
	Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::BloomFinal]->activate(dc);
	Graphics::Instance().bloomer->make(dc, Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::screenquad]->shader_resource_views[0].Get());

	//dc->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
	//dc->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
	//dc->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
	ID3D11ShaderResourceView* shader_resource_views[] =
	{
		Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::screenquad]->shader_resource_views[0].Get(),
		Graphics::Instance().bloomer->shader_resource_view(),
	};
	Graphics::Instance().bit_block_transfer->blit(dc, shader_resource_views, 10, 2, Graphics::Instance().pixel_shaders[(int)Graphics::PPShaderType::BloomFinal].Get());
		minimap->Render(player->GetPosition());
	
	Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::BloomFinal]->deactivate(dc);

	Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::crt]->clear(dc);
	Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::crt]->activate(dc);

	Graphics::Instance().bit_block_transfer->blit(dc, shader_resource_views, 10, 2, Graphics::Instance().pixel_shaders[(int)Graphics::PPShaderType::BloomFinal].Get());
	Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::crt]->deactivate(dc);

	Graphics::Instance().bit_block_transfer->blit(
		dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BloomFinal)]->shader_resource_views[0].GetAddressOf(), 10, 1);
}

// GUI描画
void SceneGame::DrawGUI()
{
	minimap->DrawImGui();
}
