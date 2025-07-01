#include "System/Graphics.h"
#include "SceneGraphics.h"
#include "Camera/Camera.h"
#include "System/GamePad.h"
#include "System/Input.h"

#include <imgui.h>

CONST LONG SHADOWMAP_WIDTH = { 2048 };
CONST LONG SHADOWMAP_HEIGHT = { 2048 };


void SceneGraphics::SetPointLightData()
{
	// 点光源の設定
	pointLights[0].position = { 0,1,1,0 };
	pointLights[0].color = { 1,1,1,0 };
	pointLights[0].range = { 5 };

	//pointLights[1].position = { 1,1,-1,0 };
	//pointLights[1].color = { 1,0,0,1 };
	//pointLights[1].range = { 10 };

	//pointLights[2].position = { -1,1,1,0 };
	//pointLights[2].color = { 0,1,0,1 };
	//pointLights[2].range = { 10 };

	//pointLights[3].position = { -1,1,-1,0 };
	//pointLights[3].color = { 0,0,1,1 };
	//pointLights[3].range = { 10 };

	ZeroMemory(&pointLights[1], sizeof(PointLightConstants) * 7);

	// 線光源の設定
	lineLights[0].start = { 0,2,0,0 };
	lineLights[0].end = { 2,2,0,0 };
	lineLights[0].color = { 1,1,1,0 };
	lineLights[0].range = { 5 };

	//lightData[0].position = { 0,1,0 };
	//lightData[0].angle = { 0,0,0 };

	ZeroMemory(&lineLights[1], sizeof(LineLightConstants) * 7);
}


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
	GamePad& gamepad = Input::Instance().GetGamePad(); ///< ゲームパッド入力

	/// ステージとプレイヤーの更新
	stage->Update(elapsedTime);
	player->Update(elapsedTime);

	/// カメラコントローラーの種類による分岐
	// 一人称カメラコントローラーの場合
	if (typeid(*i_CameraController) == typeid(FPCameraController))
	{
		/// 画面中央の座標を取得し、マウスカーソルを中央に移動
		POINT screenPoint = { Input::Instance().GetMouse().GetScreenWidth() / 2, Input::Instance().GetMouse().GetScreenHeight() / 2 };
		ClientToScreen(Graphics::Instance().GetWindowHandle(), &screenPoint);

		/// プレイヤーの位置をカメラ位置に設定
		DirectX::XMFLOAT3 cameraPos = player->GetPosition();
		i_CameraController->SetCameraPos(cameraPos);

		/// カメラコントローラーの更新
		i_CameraController->Update(elapsedTime);

		/// マウスカーソルを画面中央に移動
		SetCursorPos(screenPoint.x, screenPoint.y);

		/// CTRL+Xボタンでフリーカメラに切り替え
		if (gamepad.GetButton() & GamePad::CTRL && gamepad.GetButtonDown() & GamePad::BTN_X)
		{
			i_CameraController = std::make_unique<FreeCameraController>();
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

	/// グラフィックスの定数バッファ更新
	Graphics::Instance().UpdateConstantBuffer(elapsedTime);
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
	rc.lightDirection = { 0.0f, -1.0f, 0.0f }; ///< ライトの方向（下方向）
	rc.renderState = graphics.GetRenderState(); ///< レンダーステート

	/// カメラパラメータの設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView(); ///< ビュー行列
	rc.projection = camera.GetProjection(); ///< 射影行列
#if 1
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

	/// フレームバッファのクリアとアクティベート（ポストプロセス用）
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->clear(dc, 0.5f, 0.5f, 1, 1);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->activate(dc);

	/// 3Dモデル描画処理
	{
		/// ステージの描画
		stage->Render(rc, modelRenderer);

		/// プレイヤーの描画
		player->Render(rc, modelRenderer);
	}

	/// 3Dデバッグ描画処理
	{
		/// プレイヤーのデバッグ描画（ボックス・カプセル表示）
		player->RenderDebug(rc, shapeRenderer, { 1,2,1 }, { 1,1,1,1 }, DEBUG_MODE::BOX | DEBUG_MODE::CAPSULE);

		/// 点光源のデバッグ描画
		for (auto& p : pointLights) {
			shapeRenderer->RenderSphere(rc, { p.position.x, p.position.y, p.position.z }, 0.5f, { 1, 0, 0, 1 });
		}
		/// 線光源のデバッグ描画
		for (int i = 0; i < 8; i++) {
			shapeRenderer->RenderBox(rc, lightData[i].position, lightData[i].angle, { 1,0.4f,0.4f }, { 0,0,1,1 });
		}
	}

	/// 2Dスプライト描画処理（未実装）
	{
		// ここに2Dスプライト描画処理を追加可能
	}

	shadow->Release(dc);

	/// フレームバッファのディアクティベート
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->deactivate(dc);

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

#if 1
	/// デバッグ用：ポストプロセス結果の描画
	Graphics::Instance().bit_block_transfer->blit(
		dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->shader_resource_views[0].GetAddressOf(), 10, 1
	);
#endif
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

	if (ImGui::TreeNode("Light"))
	{
		ImGui::SliderFloat("lightPower", &lightPower, 0, 100);
		if (ImGui::TreeNode("pointLight"))
		{
			for (int i = 0; i < 8; ++i) {
				std::string p = std::string("position") + std::to_string(i);
				ImGui::DragFloat3(p.c_str(), &pointLights[i].position.x, -30.0f, +30.0f);
				std::string c = std::string("color") + std::to_string(i);
				ImGui::ColorEdit3(c.c_str(), &pointLights[i].color.x);
				std::string r = std::string("range") + std::to_string(i);
				ImGui::DragFloat3(r.c_str(), &pointLights[i].range, 0.0f, +100.0f);
			}

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("lineLight"))
		{

			for (int i = 0; i < 8; ++i) {

				DirectX::XMMATRIX M = DirectX::XMMatrixRotationRollPitchYaw(lightData[i].angle.x, lightData[i].angle.y, lightData[i].angle.z);
				DirectX::XMVECTOR Dir = M.r[0];
				DirectX::XMFLOAT3 direction;
				DirectX::XMStoreFloat3(&direction, Dir);

				DirectX::XMFLOAT3 start, end;
				//DirectX::XMVECTOR Length = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(&lineLights[i].end), DirectX::XMLoadFloat4(&lineLights[i].start));
				//Length = DirectX::XMVector3Length(Length);
				//float length = DirectX::XMVectorGetX(Length);

				start.x = lightData[i].position.x + direction.x * (lightData[i].length * 0.5f);
				start.y = lightData[i].position.y + direction.y * (lightData[i].length * 0.5f);
				start.z = lightData[i].position.z + direction.z * (lightData[i].length * 0.5f);
				end.x = lightData[i].position.x - direction.x * (lightData[i].length * 0.5f);
				end.y = lightData[i].position.y - direction.y * (lightData[i].length * 0.5f);
				end.z = lightData[i].position.z - direction.z * (lightData[i].length * 0.5f);

				lineLights[i].start.x = start.x;
				lineLights[i].start.y = start.y;
				lineLights[i].start.z = start.z;
				lineLights[i].end.x = end.x;
				lineLights[i].end.y = end.y;
				lineLights[i].end.z = end.z;


				//std::string s = std::string("start") + std::to_string(i);
				//ImGui::SliderFloat3(s.c_str(), &lineLights[i].start.x, -10.0f, +30.0f);
				//std::string e = std::string("end") + std::to_string(i);
				//ImGui::SliderFloat3(e.c_str(), &lineLights[i].end.x, -10.0f, +30.0f);
				std::string s = std::string("pos") + std::to_string(i);
				ImGui::DragFloat3(s.c_str(), &lightData[i].position.x, -30.0f, +30.0f);
				std::string l = std::string("length") + std::to_string(i);
				ImGui::DragFloat(l.c_str(), &lightData[i].length, -10.0f, +10.0f);
				std::string a = std::string("angle") + std::to_string(i);
				ImGui::DragFloat3(a.c_str(), &lightData[i].angle.x, -30.0f, +30.0f);
				std::string c = std::string("color") + std::to_string(i);
				ImGui::ColorEdit3(c.c_str(), &lineLights[i].color.x);
				std::string r = std::string("range") + std::to_string(i);
				ImGui::DragFloat(r.c_str(), &lineLights[i].range, 0.0f, +100.0f);
			}

			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void SceneGraphics::UpdateConstants(RenderContext& rc)
{
	rc.lightDirection = lightDirection;	// (ToT)
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

	// ポイントライトの設定
	rc.lightPower = lightPower;
	for (int i = 0; i < 8; ++i) {
		rc.pointLights[i].position = pointLights[i].position;
		rc.pointLights[i].color = pointLights[i].color;
		rc.pointLights[i].range = pointLights[i].range;
		//rc.pointLights[i].dummy = { 0,0,0 };
	}
	for (int i = 0; i < 8; ++i) {
		rc.lineLights[i].start = lineLights[i].start;
		rc.lineLights[i].end = lineLights[i].end;
		rc.lineLights[i].color = lineLights[i].color;
		rc.lineLights[i].range = lineLights[i].range;
		//rc.lineLights[i].dummy = { 0,0,0 };
	}
}
