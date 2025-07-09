#include "fujimoto.h"
#include "System/Graphics.h"
#include "Camera/Camera.h"
#include "System/GamePad.h"
#include "System/Input.h"
#include "Pursuer/SearchAI.h"
#include "imgui.h"                    // ImGuiの基本機能
#include "imgui_impl_win32.h"        // Win32用バックエンド
#include "imgui_impl_dx11.h"         // DirectX11用バックエンド
#include "./LightModels/LightManager.h"


// 初期化
void fujimoto::Initialize()
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

	enemy = std::make_shared<Enemy>(player, stage);
}

// 終了化
void fujimoto::Finalize()
{
	//ステージ終了化
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}
}

// 更新処理
void fujimoto::Update(float elapsedTime)
{
	GamePad& gamepad = Input::Instance().GetGamePad();

	//ステージ更新処理
	stage->Update(elapsedTime);
	player->Update(elapsedTime);
	enemy->Update(elapsedTime);

	/// カメラコントローラーの種類による分岐
	// 一人称カメラコントローラーの場合
	if (typeid(*i_CameraController) == typeid(FPCameraController))
	{
		/// 画面中央の座標を取得し、マウスカーソルを中央に移動
		POINT screenPoint = { Input::Instance().GetMouse().GetScreenWidth() / 2, Input::Instance().GetMouse().GetScreenHeight() / 2 };
		ClientToScreen(Graphics::Instance().GetWindowHandle(), &screenPoint);

		/// プレイヤーの位置をカメラ位置に設定
		DirectX::XMFLOAT3 cameraPos = player->GetPosition();
		cameraPos.y = 1.5f;
		i_CameraController->SetCameraPos(cameraPos);

		/// カメラコントローラーの更新
		i_CameraController->Update(elapsedTime);

		/// マウスカーソルを画面中央に移動
		SetCursorPos(screenPoint.x, screenPoint.y);

		/// CTRL+Xボタンでフリーカメラに切り替え
		if (gamepad.GetButton() & GamePad::CTRL && gamepad.GetButtonDown() & GamePad::BTN_X)
		{
			//i_CameraController = std::make_unique<FreeCameraController>();
			i_CameraController = std::make_unique<LightDebugCameraController>();
		}
	}
	// フリーカメラコントローラーの場合
	else
	{
		/// カメラコントローラーの更新
		i_CameraController->Update(elapsedTime);

		/// CTRL+Xボタンで一人称カメラに切り替え
		if (gamepad.GetButton() & GamePad::CTRL && gamepad.GetButtonDown() & GamePad::BTN_X)
		{
			i_CameraController = std::make_unique<FPCameraController>();
		}
	}
}

// 描画処理
void fujimoto::Render()
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

	// 3Dモデル描画
	{
		//ステージ描画
		stage->Render(rc, modelRenderer);

		player->Render(rc, modelRenderer);

		enemy->Render(rc, modelRenderer);
	}

	// 3Dデバッグ描画
	{
		player->RenderDebug(rc, shapeRenderer, { 1,2,1 }, { 1,1,1,1 }, DEBUG_MODE::BOX | DEBUG_MODE::CAPSULE);
	}

	// 2Dスプライト描画
	{

	}

	{
		ImGui::Begin("Scene Info");

		// positionを表示
		ImGui::Text("Position: X=%.2f, Y=%.2f, Z=%.2f",
			player->GetPosition().x,
			player->GetPosition().y,
			player->GetPosition().z);

		ImGui::Text("index: X=%d",
			enemy.get()->GetindexWayPoint());


		/*ImGui::Text("Position: X=%.2f, Y=%.2f, Z=%.2f",
			i_CameraController->GetTarget().x,
			i_CameraController->GetTarget().y,
			i_CameraController->GetTarget().z);*/
		ImGui::End();

	}
}

// GUI描画
void fujimoto::DrawGUI()
{
	enemy->DrawDebug();
}
