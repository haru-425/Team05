#include "System/Graphics.h"
#include "Camera/Camera.h"
#include "System/GamePad.h"
#include "System/Input.h"
#include"Scene/SceneGameOver.h"
#include"Scene/SceneManager.h"
#include "Collision.h"
#include "./LightModels/LightManager.h"
#include "./Aircon/AirconManager.h"
#include "fujimoto.h"
#include "Pursuer/SearchAI.h"
#include "GameObjects/battery/batteryManager.h"
#include "GameObjects/Object/ObjectManager.h"
#include "imgui.h"                    // ImGuiの基本機能
#include "imgui_impl_win32.h"        // Win32用バックエンド
#include "imgui_impl_dx11.h"         // DirectX11用バックエンド


#include <imgui.h>

CONST LONG SHADOWMAP_WIDTH = { 2048 };
CONST LONG SHADOWMAP_HEIGHT = { 2048 };

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
	//i_CameraController = std::make_unique<FPCameraController>();
	i_CameraController = std::make_unique<LightDebugCameraController>();

	player = std::make_shared<Player>();

	enemy = std::make_shared<Enemy>(player, stage);

	//ミニマップスプライト初期化
	minimap = new MiniMap();
	timer = 0.0f; // タイマー初期化
	transTimer = 0.0f; // シーン遷移タイマー初期化

	selectTrans = SelectTrans::GameOver; // シーン遷移選択初期化
	sceneTrans = false; // シーン遷移フラグ初期化

	// shadowMap
	ID3D11Device* device = Graphics::Instance().GetDevice();
	shadow = std::make_unique<ShadowCaster>(device, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);

	// ライトの初期化
	LightManager::Instance().Initialize();

	// エアコンの初期化
	AirconManager::Instance().Initialize();

	batteryManager::Instance().addBattery({ 0,0,0 });
	batteryManager::Instance().SetPlayer_and_enemy(player, enemy);
	batteryManager::Instance().ResetPlayer_get_Batterry();
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
	//ミニマップ終了化
	if (minimap != nullptr)
	{
		delete minimap;
		minimap = nullptr;
	}
}

// 更新処理
void fujimoto::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// 任意のゲームパッドボタンが押されているか
	const GamePadButton anyButton =
		GamePad::BTN_A
		| GamePad::BTN_B
		| GamePad::BTN_X
		| GamePad::BTN_Y;

	bool buttonPressed = (anyButton & gamePad.GetButton()) != 0;
	bool zKey = GetAsyncKeyState('Z') & 0x8000;

	// フラグがまだ立っていない場合に入力検出
	if (!sceneTrans)
	{
		if (zKey)
		{
			nextScene = new Game_Over;
			sceneTrans = true;
			transTimer = 0.0f;
			selectTrans = SelectTrans::GameOver; // ゲームオーバーシーンに遷移
		}

	}
	else
	{
		// フラグが立っている間タイマーを加算し、1秒以上経ったらシーン切り替え
		transTimer += elapsedTime;
		if (transTimer >= 3.0f && nextScene != nullptr)
		{
			SceneManager::instance().ChangeScene(nextScene);
			nextScene = nullptr; // 多重遷移防止
			sceneTrans = false; // シーン遷移フラグをリセット
		}
	}

	timer += elapsedTime;
	Graphics::Instance().UpdateConstantBuffer(timer, transTimer);

	////ゲームオーバーに強制遷移
	//if (GetAsyncKeyState('Z') & 0x8000)
	//{
	//	// Zキーが押されているときに実行される
	//	SceneManager::instance().ChangeScene(new Game_Over);
	//}

	//ステージ更新処理
	stage->Update(elapsedTime);
	player->Update(elapsedTime);
	enemy->Update(elapsedTime);
	minimap->Update(player->GetPosition());

	// 一人称用カメラ
	if (typeid(*i_CameraController) == typeid(FPCameraController))
	{
		POINT screenPoint = { Input::Instance().GetMouse().GetScreenWidth() / 2, Input::Instance().GetMouse().GetScreenHeight() / 2 };
		ClientToScreen(Graphics::Instance().GetWindowHandle(), &screenPoint);
		DirectX::XMFLOAT3 cameraPos = player->GetPosition();
		cameraPos.y = player->GetViewPoint();
		i_CameraController->SetCameraPos(cameraPos);
		i_CameraController->Update(elapsedTime);
		SetCursorPos(screenPoint.x, screenPoint.y);

		if (gamePad.GetButton() & GamePad::CTRL && gamePad.GetButtonDown() & GamePad::BTN_X)
		{
			i_CameraController = std::make_unique<FreeCameraController>();
		}
	}
	// フリーカメラ
	else
	{
		i_CameraController->Update(elapsedTime);

		if (gamePad.GetButton() & GamePad::CTRL && gamePad.GetButtonDown() & GamePad::BTN_X)
		{
			i_CameraController = std::make_unique<FPCameraController>();
		}
	}
	Graphics::Instance().UpdateConstantBuffer(timer, transTimer);


	LightManager::Instance().Update();

	batteryManager::Instance().Update(elapsedTime);

	ObjectManager::Instance().Update(elapsedTime);
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
	// 定数の更新
	UpdateConstants(rc);
	LightManager::Instance().UpdateConstants(rc);
	/*Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->clear(dc, 0.5f, 0.5f, 1, 1);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->activate(dc);*/
	// 3Dモデル描画
	{
		//ステージ描画
		stage->Render(rc, modelRenderer);

		player->Render(rc, modelRenderer);

		enemy->Render(rc, modelRenderer);

		LightManager::Instance().Render(rc);

		//AirconManager::Instance().Render(rc);

		batteryManager::Instance().Render(rc, modelRenderer);

		ObjectManager::Instance().Render(rc, modelRenderer);
	}

	// 3Dデバッグ描画
	{
		player->RenderDebug(rc, shapeRenderer, { 1,2,1 }, { 1,1,1,1 }, DEBUG_MODE::BOX | DEBUG_MODE::CAPSULE);

		//enemy->RenderDebug(rc, shapeRenderer, { 1,2,1 }, { 1,1,1,1 }, DEBUG_MODE::BOX | DEBUG_MODE::CAPSULE);

	}

	/// 当たり判定の更新
	Collision();

	player->UpdateTransform();

	// 2Dスプライト描画
	{
		//minimap->Render(player->GetPosition());
	}

}

// GUI描画
void fujimoto::DrawGUI()
{
	minimap->DrawImGui();
	LightManager::Instance().DebugGUI();
	RenderContext rc;

	ImGui::Separator();

	// (ToT)
	ImGui::SliderFloat3("lightDirection", reinterpret_cast<float*>(&lightDirection), -1.0f, +1.0f);
	ImGui::DragFloat("shadowMapDrawRect", &SHADOWMAP_DRAWRECT, 0.1f);

	// shadow->DrawGUI();

	ImGui::Separator();

	if (ImGui::TreeNode("texture"))
	{
		ImGui::Text("shadow_map");
		//ImGui::Image(shadowShaderResourceView.Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::DragFloat("shadowBias", &shadowBias, 0.0001f, 0, 1, "%.6f");
		ImGui::ColorEdit3("shadowColor", reinterpret_cast<float*>(&shadowColor));

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("ambient"))
	{
		ImGui::InputFloat3("cameraPosition", &cameraPosition.x);
		ImGui::ColorEdit3("ambient_color", &ambientColor.x);
		ImGui::ColorEdit3("fog_color", &fogColor.x);
		ImGui::DragFloat("fog_near", &fogRange.x, 0.1f, +100.0f);
		ImGui::DragFloat("fog_far", &fogRange.y, 0.1f, +100.0f);


		ImGui::TreePop();
	}/*
	Graphics::Instance().DebugGUI();
	LightManager::Instance().DebugGUI();
	AirconManager::Instance().DebugGUI();*/
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

void fujimoto::Collision()
{
	/// プレイヤーとステージとの当たり判定
	PlayerVsStage();
}

void fujimoto::PlayerVsStage()
{
	using namespace DirectX;

	const XMFLOAT3 playerPos = player->GetPosition();
	const XMFLOAT3 playerDir = player->GetDirection();
	XMFLOAT3 playerRight;
	const XMFLOAT3 up = { 0,1,0 };
	XMStoreFloat3(&playerRight, XMVector3Cross(XMLoadFloat3(&up), XMLoadFloat3(&playerDir)));


	const XMFLOAT3 rayStart = { playerPos.x, playerPos.y, playerPos.z };
	const XMFLOAT3 rayEndF = { playerPos.x + playerDir.x, playerPos.y, playerPos.z + playerDir.z };
	const XMFLOAT3 rayEndR = { playerPos.x + playerRight.x, playerPos.y, playerPos.z + playerRight.z };

	DirectX::XMFLOAT3 hitPosition, hitNormal;
	if (Collision::RayCast(rayStart, rayEndF, stage->GetCollisionMatrix(), stage->GetCollisionMesh(), hitPosition, hitNormal))
	{
		XMVECTOR P = XMLoadFloat3(&hitPosition);
		XMVECTOR E = XMLoadFloat3(&rayEndF);
		XMVECTOR PE = XMVectorSubtract(E, P);

		// 三角関数で終点から壁までの長さを求める
		XMVECTOR N = XMLoadFloat3(&hitNormal);
		// PEの終点にNベクトルを持ってくる
		// 正規化したNとPEで内積
		XMVECTOR NegatePE = XMVectorNegate(PE); // このままPEとAで内積するとおかしくなっちゃうからPEの逆ベクトルを求める
		N = XMVector3Normalize(N);
		XMVECTOR A = XMVector3Dot(NegatePE, N); // 射影長を求める
		//XMVECTOR A = XMVector3Dot(XMVectorNegate(PE), N);

		// 壁までの長さを少しだけ長くなるように補正する
		float a = XMVectorGetX(A) + 0.01f;

		// 壁刷りのベクトルを求める
		A = XMVectorScale(N, a);
		XMVECTOR R = XMVectorAdd(PE, A);
		//XMVECTOR R = XMVectorAdd(PE, XMVectorScale(N, a));
		//XMVECTOR R = XMVector3Dot(XMVectorNegate(PE), N);

		// 壁刷り後の位置を求める
		XMVECTOR Q = XMVectorAdd(P, R);
		XMFLOAT3 q;
		XMStoreFloat3(&q, Q);

		XMFLOAT3 playerPos = player->GetPosition();
#if 0
		playerPos.x = q.x - playerDir.x;
		playerPos.z = q.z - playerDir.z;
#else
		playerPos.x = q.x;
		playerPos.z = q.z;
#endif
		player->SetPosition(playerPos);
	}
}

void fujimoto::UpdateCamera(float elapsedTime)
{
}

void fujimoto::UpdateConstants(RenderContext& rc)
{
	rc.lightDirection = lightDirection;	// (ToT)+
	// シャドウの設定
	rc.shadowColor = shadowColor;
	rc.shadowBias = shadowBias;

	// フォグの設定
	rc.ambientColor = ambientColor;
	rc.fogColor = fogColor;
	rc.fogRange = fogRange;

	//カメラパラメータ設定
	Camera& camera = Camera::Instance();
	cameraPosition = camera.GetEye();
	rc.cameraPosition.x = cameraPosition.x;
	rc.cameraPosition.y = cameraPosition.y;
	rc.cameraPosition.z = cameraPosition.z;

	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();
}
