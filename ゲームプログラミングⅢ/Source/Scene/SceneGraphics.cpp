#include "System/Graphics.h"
#include "SceneGraphics.h"
#include "Camera/Camera.h"
#include "System/GamePad.h"
#include "System/Input.h"
#include "./LightModels/LightManager.h"
#include "Aircon/AirconManager.h"
#include "TV/TV.h"
#include "Object/ObjectManager.h"

#include <imgui.h>

CONST LONG SHADOWMAP_WIDTH = { 2048 };
CONST LONG SHADOWMAP_HEIGHT = { 2048 };



/**
 * @file SceneGraphics.cpp
 * @brief シーングラフィックスの初期化、更新、描画、GUI描画を行うクラスの実装ファイル
 * @author
 * @date
 *
 * 本ファイルでは、シーン内のグラフィックスに関する主要な処理（初期化、終了化、更新、描画、GUI描画）を実装しています。
 * 各関数にはDoxygen形式で詳細な説明を記載しています。
 *
 * @see SceneGraphics.h
 */

 /**
  * @brief シーングラフィックスの初期化処理を行います。
  *
  * ステージ、カメラ、カメラコントローラー、プレイヤーの初期化を行います。
  *
  * @details
  * - ステージのインスタンスを生成します。
  * - カメラの初期位置・注視点・上方向を設定します。
  * - 一人称カメラコントローラー（FPCameraController）を生成します。
  * - プレイヤーのインスタンスを生成します。
  *
  * @see Stage, Camera, FPCameraController, Player
  */
void SceneGraphics::Initialize()
{
	/// ステージのインスタンス生成
	stage = new Stage();

	/// カメラの初期設定
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10), ///< カメラの初期位置
		DirectX::XMFLOAT3(0, 0, 0),    ///< 注視点
		DirectX::XMFLOAT3(0, 1, 0)     ///< 上方向ベクトル
	);

	/// 一人称カメラコントローラーの生成
	i_CameraController = std::make_unique<FPCameraController>();

	/// プレイヤーのインスタンス生成
	player = std::make_shared<Player>();

	// shadowMap
	ID3D11Device* device = Graphics::Instance().GetDevice();
	shadow = std::make_unique<ShadowCaster>(device, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);

	// ゲームオブジェクトの初期化
	LightManager::Instance().Initialize();
	AirconManager::Instance().Initialize();
	//TV::Instance().Initialize();
	ObjectManager::Instance().Initialize();

	/// デバッグ用タイマー初期化
	time = 0;
	// 3Dオーディオシステムにエミッターを追加
	//BGM


	// リスナーの初期位置と向きを設定
	Audio3DSystem::Instance().UpdateListener(Camera::Instance().GetEye(), Camera::Instance().GetFront(), Camera::Instance().GetUp());
	// 3Dオーディオシステムの再生開始
	Audio3DSystem::Instance().PlayByTag("atmosphere_noise");
	Audio3DSystem::Instance().PlayByTag("aircon");
}

/**
 * @brief シーングラフィックスの終了化処理を行います。
 *
 * ステージのメモリ解放を行います。
 *
 * @details
 * - ステージが存在する場合はdeleteし、ポインタをnullptrにします。
 *
 * @see Stage
 */
void SceneGraphics::Finalize()
{
	/// ステージのメモリ解放
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}
	Audio3DSystem::Instance().StopByTag("atmosphere_noise"); // 音声停止
	Audio3DSystem::Instance().StopByTag("aircon"); // 音声停止
}

/**
 * @brief シーングラフィックスの更新処理を行います。
 *
 * ステージ、プレイヤー、カメラコントローラーの更新を行い、カメラの切り替えも管理します。
 *
 * @param elapsedTime 前フレームからの経過時間（秒）
 *
 * @details
 * - 入力デバイスの取得
 * - ステージとプレイヤーの更新
 * - カメラコントローラーの種類に応じてカメラの更新と切り替え
 * - グラフィックスの定数バッファ更新
 *
 * @see Stage::Update, Player::Update, ICameraController, FPCameraController, FreeCameraController
 */
void SceneGraphics::Update(float elapsedTime)
{
	//Sleep(10);


	GamePad& gamepad = Input::Instance().GetGamePad(); ///< ゲームパッド入力

	/// ステージとプレイヤーの更新
	stage->Update(elapsedTime);
	player->Update(elapsedTime);

	/// カメラコントローラーの種類による分岐
	// 一人称カメラコントローラーの場合
	if (typeid(*i_CameraController) == typeid(FPCameraController))
	{
		//i_CameraController = std::make_unique<LightDebugCameraController>();
#if 1
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
#endif
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

	/// グラフィックスの定数バッファ更新
	/// 時間の更新（デバッグ用）
	time += elapsedTime; ///< ミリ秒単位で時間を更新

	Graphics::Instance().UpdateConstantBuffer(time);

	LightManager::Instance().Update();
	//TV::Instance().Update(elapsedTime);
	ObjectManager::Instance().Update(elapsedTime);

	Audio3DSystem::Instance().SetEmitterPositionByTag("atmosphere_noise", Camera::Instance().GetEye());
	Audio3DSystem::Instance().UpdateListener(Camera::Instance().GetEye(), Camera::Instance().GetFront(), Camera::Instance().GetUp());
	Audio3DSystem::Instance().UpdateEmitters(elapsedTime);
}

/**
 * @brief シーングラフィックスの描画処理を行います。
 *
 * ステージ、プレイヤーの3Dモデル描画、デバッグ描画、ポストプロセス処理を行います。
 *
 * @details
 * - カメラの射影行列設定
 * - 描画用コンテキスト(RenderContext)の準備
 * - ステージ・プレイヤーの3Dモデル描画
 * - プレイヤーのデバッグ描画
 * - ポストプロセス（ハイライト、ブラー等）の適用
 *
 * @see RenderContext, ModelRenderer, ShapeRenderer, Camera, Player, Stage
 */
void SceneGraphics::Render()
{
	Graphics& graphics = Graphics::Instance(); ///< グラフィックス管理インスタンス
	ID3D11DeviceContext* dc = graphics.GetDeviceContext(); ///< デバイスコンテキスト
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer(); ///< 形状描画用レンダラー
	ModelRenderer* modelRenderer = graphics.GetModelRenderer(); ///< モデル描画用レンダラー

	/// カメラの射影行列を設定
	Camera::Instance().SetPerspectiveFov(
		45, ///< 視野角（度）
		graphics.GetScreenWidth() / graphics.GetScreenHeight(), ///< アスペクト比
		0.1f, ///< ニアクリップ
		1000.0f ///< ファークリップ
	);

	/// 描画用コンテキストの準備
	RenderContext rc;
	rc.deviceContext = dc; ///< デバイスコンテキスト
	rc.lightDirection = lightDirection; ///< ライトの方向（下方向）
	rc.renderState = graphics.GetRenderState(); ///< レンダーステート

	/// カメラパラメータの設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView(); ///< ビュー行列
	rc.projection = camera.GetProjection(); ///< 射影行列
#if 0
	// shadow
	{
		Camera& camera = Camera::Instance();

		// ライトの位置から見た視線行列を生成
		DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&lightDirection); // TODO : pointLightにするときはここを変更
		LightPosition = DirectX::XMVectorScale(LightPosition, -50);
		DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(LightPosition,
			DirectX::XMVectorSet(camera.GetFocus().x, camera.GetFocus().y, camera.GetFocus().z, 1.0f), // (ToT)
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		// シャドウマップに描画したい範囲の射影行列を生成
		DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(SHADOWMAP_DRAWRECT, SHADOWMAP_DRAWRECT,
			0.1f, 200.0f);

		DirectX::XMStoreFloat4x4(&rc.view, V);
		DirectX::XMStoreFloat4x4(&rc.projection, P);
		DirectX::XMStoreFloat4x4(&rc.lightViewProjection, V * P);

		shadow->Clear(dc, 1.0f);
		shadow->Active(dc);

		// 3Dモデル描画
		{
			/// ステージの描画
			//stage->Render(rc, modelRenderer);
		}
		shadow->Deactive(dc);
	}
#endif
	// 定数の更新
	UpdateConstants(rc);
	LightManager::Instance().UpdateConstants(rc);

	/// フレームバッファのクリアとアクティベート（ポストプロセス用）
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->clear(dc, 0.5f, 0.5f, 1, 1);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->activate(dc);

	/// 3Dモデル描画処理
	{
		/// ステージの描画
		stage->Render(rc, modelRenderer);

		/// プレイヤーの描画
		player->Render(rc, modelRenderer);

		/// ライトモデルの描画
		LightManager::Instance().Render(rc);

		AirconManager::Instance().Render(rc);

		//TV::Instance().Render(rc, modelRenderer);

		ObjectManager::Instance().Render(rc, modelRenderer);
	}

	/// 3Dデバッグ描画処理
	{
		/// プレイヤーのデバッグ描画（ボックス・カプセル表示）
		player->RenderDebug(rc, shapeRenderer, { 1,2,1 }, { 1,1,1,1 }, DEBUG_MODE::BOX | DEBUG_MODE::CAPSULE);

		/// ライトモデルのデバッグ描画
		//LightManager::Instance().RenderDebugPrimitive(rc);

		/// エアコンのデバッグ描画
		//AirconManager::Instance().RenderDebugPrimitive(rc);

		ObjectManager::Instance().RenderDebugPrimitive(rc, shapeRenderer);
	}

	/// 2Dスプライト描画処理（未実装）
	{
		// ここに2Dスプライト描画処理を追加可能
	}

	//shadow->Release(dc);

	/// フレームバッファのディアクティベート
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->deactivate(dc);
#if 0
	/// ハイライトパス用フレームバッファのクリアとアクティベート
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::HighLightPass)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::HighLightPass)]->activate(dc);

	/// ハイライトパスのポストプロセス適用
	Graphics::Instance().bit_block_transfer->blit(
		dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->shader_resource_views[0].GetAddressOf(),
		10, 1,
		Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::HighLightPass)].Get()
	);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::HighLightPass)]->deactivate(dc);

	/// ブラーパスのポストプロセス適用
	ID3D11ShaderResourceView* shader_resource_views[2] = {
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->shader_resource_views[0].Get(),
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::HighLightPass)]->shader_resource_views[0].Get()
	};

	Graphics::Instance().bit_block_transfer->blit(
		dc,
		shader_resource_views, 10, 2,
		Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::Blur)].Get()
	);

	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BreathShake)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BreathShake)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		&shader_resource_views[0], 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::BreathShake)].Get());

	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BreathShake)]->deactivate(dc);

	//TEMPORAL NOISE
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BreathShake)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::TemporalNoise)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->deactivate(dc);

	//// WardenGaze
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::WardenGaze)]->clear(dc);
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::WardenGaze)]->activate(dc);
	//Graphics::Instance().bit_block_transfer->blit(dc,
	//	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::WardenGaze)].Get());
	//{

	//	sprite->Render(rc, 0, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
	//}
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::WardenGaze)]->deactivate(dc);


	//// Sharpen
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Sharpen)]->clear(dc);
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Sharpen)]->activate(dc);
	//Graphics::Instance().bit_block_transfer->blit(dc,
	//	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::Sharpen)].Get());
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Sharpen)]->deactivate(dc);


	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);

	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);


#endif
#if 0
	/// デバッグ用：ポストプロセス結果の描画
	Graphics::Instance().bit_block_transfer->blit(
		dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1
	);
#endif

	// BLOOM
	Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::BloomFinal]->clear(dc);
	Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::BloomFinal]->activate(dc);
	Graphics::Instance().bloomer->make(dc, Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::screenquad]->shader_resource_views[0].Get());

	ID3D11ShaderResourceView* shader_resource_views[] =
	{
		Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::screenquad]->shader_resource_views[0].Get(),
		Graphics::Instance().bloomer->shader_resource_view(),
	};
	Graphics::Instance().bit_block_transfer->blit(dc, shader_resource_views, 10, 2, Graphics::Instance().pixel_shaders[(int)Graphics::PPShaderType::BloomFinal].Get());
	Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::BloomFinal]->deactivate(dc);
	////TemporalNoise
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->clear(dc);
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->activate(dc);
	//Graphics::Instance().bit_block_transfer->blit(dc,
	//	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BloomFinal)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::TemporalNoise)].Get());
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->deactivate(dc);
	////FilmGrainDustPS
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->clear(dc);
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->activate(dc);
	//Graphics::Instance().bit_block_transfer->blit(dc,
	//	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::FilmGrainDust)].Get());
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->deactivate(dc);

	////crt
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);

	//Graphics::Instance().bit_block_transfer->blit(dc,
	//	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);

	////VisionBootDown
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->clear(dc);
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->activate(dc);
	//Graphics::Instance().bit_block_transfer->blit(dc,
	//	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::VisionBootDown)].Get());
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->deactivate(dc);
	//TVNoiseFade
	/*Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::TVNoiseFade)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->deactivate(dc);*/

	//// GameOver
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->clear(dc);
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->activate(dc);
	//Graphics::Instance().bit_block_transfer->blit(dc,
	//	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::GameOver)].Get());
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::GameOver)]->deactivate(dc);


	//LightFlicker
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::LightFlicker)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::LightFlicker)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BloomFinal)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::LightFlicker)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::LightFlicker)]->deactivate(dc);

	//crt

	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::LightFlicker)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);
	////NoiseChanse
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoiseChange)]->clear(dc);
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoiseChange)]->activate(dc);
	//Graphics::Instance().bit_block_transfer->blit(dc,
	//	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::NoiseChange)].Get());
	//Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoiseChange)]->deactivate(dc);
	Graphics::Instance().bit_block_transfer->blit(
		dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1


	);

}

/**
 * @brief シーングラフィックスのGUI描画処理を行います。
 *
 * @details
 * 現状は未実装です。ImGui等のGUI描画処理を追加する場合は本関数に実装してください。
 */
void SceneGraphics::DrawGUI()
{
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
	}
	Graphics::Instance().DebugGUI();
	LightManager::Instance().DebugGUI();
	AirconManager::Instance().DebugGUI();
	ObjectManager::Instance().DebugGUI();
}

void SceneGraphics::UpdateConstants(RenderContext& rc)
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
