#include "fujimoto.h"
#include "System/Graphics.h"
#include "Camera/Camera.h"
#include "System/GamePad.h"
#include "System/Input.h"
#include "Pursuer/SearchAI.h"
#include "imgui.h"                    // ImGuiの基本機能
#include "imgui_impl_win32.h"        // Win32用バックエンド
#include "imgui_impl_dx11.h"         // DirectX11用バックエンド

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
		ImGui::Begin("Player Info");

		// positionを表示
		ImGui::Text("Position: X=%.2f, Y=%.2f, Z=%.2f",
			player->GetPosition().x,
			player->GetPosition().y,
			player->GetPosition().z);

		ImGui::Text("index: X=%d",
			enemy.get()->GetindexWayPoint());
		ImGui::End();

	}
}

// GUI描画
void fujimoto::DrawGUI()
{

}
