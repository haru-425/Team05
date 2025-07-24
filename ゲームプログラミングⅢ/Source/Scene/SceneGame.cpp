#include "System/Graphics.h"
#include "SceneGame.h"
#include "Camera/Camera.h"
#include "System/GamePad.h"
#include "System/Input.h"
#include"Scene/SceneGameOver.h"
#include"Scene/SceneClear.h"
#include"Scene/SceneManager.h"
#include "Collision.h"
#include "./LightModels/LightManager.h"
#include "./Aircon/AirconManager.h"
#include "./Object/ObjectManager.h"
#include "System/CollisionEditor.h"
#include "GameObjects/battery/batteryManager.h"

#include <imgui.h>

CONST LONG SHADOWMAP_WIDTH = { 2048 };
CONST LONG SHADOWMAP_HEIGHT = { 2048 };
float reminingTime = 300.0f;

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

	player = std::make_shared<Player>(DirectX::XMFLOAT3(1, 0, -23)); ///< プレイヤー初期化
	enemy = std::make_shared<Enemy>(player, stage); ///< 敵初期化
  metar = std::make_shared<Metar>();
	player->SetEnemy(enemy); ///< プレイヤーが敵をバインド

	//ミニマップスプライト初期化
	minimap = new MiniMap();
	timer = 0.0f; // タイマー初期化
	transTimer = 0.0f; // シーン遷移タイマー初期化
	reminingTime = 300.0f;

	selectTrans = SelectTrans::GameOver; // シーン遷移選択初期化
	sceneTrans = false; // シーン遷移フラグ初期化

	// shadowMap
	ID3D11Device* device = Graphics::Instance().GetDevice();
	shadow = std::make_unique<ShadowCaster>(device, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);

	// ライトの初期化
	LightManager::Instance().Initialize();

	// エアコンの初期化
	AirconManager::Instance().Initialize();

	ObjectManager::Instance().Initialize();

	LightManager::Instance().Update();
	Audio3DSystem::Instance().UpdateListener(Camera::Instance().GetEye(), Camera::Instance().GetFront(), Camera::Instance().GetUp());

	Audio3DSystem::Instance().SetEmitterPositionByTag("atmosphere_noise", Camera::Instance().GetEye());

	Audio3DSystem::Instance().SetEmitterPositionByTag("enemy_walk", enemy->GetPosition());
	Audio3DSystem::Instance().SetEmitterPositionByTag("enemy_run", enemy->GetPosition());
	//Audio3DSystem::Instance().SetEmitterPositionByTag("aircon", enemy->GetPosition());



	// 3Dオーディオシステムの再生開始
	//Audio3DSystem::Instance().UpdateEmitters(elapsed);
	Audio3DSystem::Instance().PlayByTag("atmosphere_noise");
	Audio3DSystem::Instance().PlayByTag("aircon");


	/// 当たり判定エディターの初期化
	CollisionEditor::Instance().Initialize();

	batteryManager::Instance().SetDifficulty(Difficulty::Instance().GetDifficulty());
	batteryManager::Instance().SetPlayer_and_Enemy(player, enemy); // バッテリーマネージャーにプレイヤーと敵を設定
	batteryManager::Instance().start();

  um.CreateUI("./Data/Sprite/back.png", "Fade");
	um.GetUIs().at(0)->GetSpriteData().color = { 0,0,0,0 };
	um.GetUIs().at(0)->GetSpriteData().isVisible = true;

	if (Difficulty::Instance().GetDifficulty() == Difficulty::mode::tutorial)
	{
		tutorial_Flug = true;
		reminingTime=120.0f;
		{
			tutorial[0] = std::make_unique<Sprite>("Data/Sprite/dialog/01.png");
			tutorial[1] = std::make_unique<Sprite>("Data/Sprite/dialog/02.png");
			tutorial[2] = std::make_unique<Sprite>("Data/Sprite/dialog/03.png");
			tutorial[3] = std::make_unique<Sprite>("Data/Sprite/dialog/04.png");
			tutorial[4] = std::make_unique<Sprite>("Data/Sprite/dialog/05.png");
			tutorial[5] = std::make_unique<Sprite>("Data/Sprite/dialog/06.png");
			tutorial[6] = std::make_unique<Sprite>("Data/Sprite/dialog/07.png");
			tutorial[7] = std::make_unique<Sprite>("Data/Sprite/dialog/08.png");
			tutorial[8] = std::make_unique<Sprite>("Data/Sprite/dialog/09.png");
			tutorial[9] = std::make_unique<Sprite>("Data/Sprite/dialog/10.png");
			tutorial[10] = std::make_unique<Sprite>("Data/Sprite/dialog/11.png");
			tutorial[11] = std::make_unique<Sprite>("Data/Sprite/dialog/12.png");
			tutorial[12] = std::make_unique<Sprite>("Data/Sprite/dialog/next_navi.png");
		}
	}
}

// 終了化
void SceneGame::Finalize()
{
	player->DeleteSounds();

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
	Audio3DSystem::Instance().StopByTag("atmosphere_noise"); // 音声停止
	Audio3DSystem::Instance().StopByTag("electrical_noise"); // 音声停止

	Audio3DSystem::Instance().StopByTag("aircon"); // 音声停止
	Audio3DSystem::Instance().StopByTag("enemy_run");
	Audio3DSystem::Instance().StopByTag("enemy_walk");

	um.Clear();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
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
	bool rKey = GetAsyncKeyState('R') & 0x8000;

	// フラグがまだ立っていない場合に入力検出
	if (!sceneTrans)
	{
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			/// シーンマネージャーに終了することを伝えて、
			/// Run 関数内で抜けるようにする
			/// exit関数はメモリリークが大量発生する可能性があるのでこの方法にする
			SceneManager::instance().SetIsExit(true);
		}
		//if (zKey)
		//{
		//	nextScene = new Game_Over(--life_number);
		//	sceneTrans = true;
		//	transTimer = 0.0f;
		//	selectTrans = SelectTrans::GameOver; // ゲームオーバーシーンに遷移
		//}
		//if (rKey)
		//{
		//	nextScene = new Game_Clear;
		//	sceneTrans = true;
		//	transTimer = 0.0f;
		//	selectTrans = SelectTrans::Clear; // ゲームオーバーシーンに遷移
		//}
		if (reminingTime <= 0.0f)
		{
			nextScene = new Game_Clear;
			sceneTrans = true;
			transTimer = 0.0f;
			selectTrans = SelectTrans::Clear; // ゲームオーバーシーンに遷移
			reminingTime = 0.0f;
			RankSystem::Instance().SetRank(
				batteryManager::Instance().getPlayerHasBattery(),
				batteryManager::Instance().getMAXBattery(),
				life_number); // タイムアップでSランク
			batteryManager::Instance().ResetPlayer_Get_Batterry();
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
#ifdef DEBUG
	reminingTime -= elapsedTime;
#endif
	Graphics::Instance().UpdateConstantBuffer(timer, transTimer, reminingTime);

	////ゲームオーバーに強制遷移
	//if (GetAsyncKeyState('Z') & 0x8000)
	//{
	//	// Zキーが押されているときに実行される
	//	SceneManager::instance().ChangeScene(new Game_Over);
	//}

  /// チュートリアル処理
  if (tutorial_Flug)
	{
		stage->Update(elapsedTime);
		minimap->Update(player->GetPosition());
		TutorialUpdate(elapsedTime);
		metar->update(player->GetenableHijackTime());
		return;
	}
  
	stage->Update(elapsedTime);						          ///< ステージ更新処理
	player->Update(elapsedTime);					          ///< プレイヤー更新処理
	enemy->Update(elapsedTime);						          ///< 敵更新処理
	minimap->Update(player->GetPosition());	        ///< ミニマップ更新処理
	batteryManager::Instance().Update(elapsedTime); ///< バッテリー更新処理
	UpdateCamera(elapsedTime);                      ///< カメラ更新処理
  metar->update(player->GetenableHijackTime());   ///< 画面左のハイジャック時間メータの更新処理 
  UpdateOneWay(elapsedTime);                      ///< 一方通行処理

	Collision(); ///< 当たり判定 

	player->UpdateTransform(); ///< プレイヤーの行列更新処理

	LightManager::Instance().Update();
	ObjectManager::Instance().Update(elapsedTime);
	Audio3DSystem::Instance().UpdateListener(Camera::Instance().GetEye(), Camera::Instance().GetFront(), Camera::Instance().GetUp());
	Audio3DSystem::Instance().SetEmitterPositionByTag("atmosphere_noise", Camera::Instance().GetEye());
	Audio3DSystem::Instance().SetEmitterPositionByTag("enemy_walk", enemy->GetPosition());
	Audio3DSystem::Instance().SetEmitterPositionByTag("enemy_run", enemy->GetPosition());

	Audio3DSystem::Instance().UpdateEmitters(elapsedTime);
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


	/// 描画用コンテキストの準備
	RenderContext rc;
	rc.deviceContext = dc; ///< デバイスコンテキスト
	rc.renderState = graphics.GetRenderState(); ///< レンダーステート

	//カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

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
			player->Render(rc, modelRenderer);
			ObjectManager::Instance().Render(rc, modelRenderer);
		}
		shadow->Deactive(dc);
	}

	// 定数の更新
	UpdateConstants(rc);
	LightManager::Instance().UpdateConstants(rc);

	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->clear(dc, 0.5f, 0.5f, 1, 1);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->activate(dc);
	// 3Dモデル描画
	{
		//ステージ描画
		stage->Render(rc, modelRenderer);

		player->Render(rc, modelRenderer);

		if (!player->GetUseCam())
			enemy->Render(rc, modelRenderer);

		LightManager::Instance().Render(rc);

		AirconManager::Instance().Render(rc);

		ObjectManager::Instance().Render(rc, modelRenderer);

		batteryManager::Instance().Render(rc, modelRenderer);
	}

	// 3Dデバッグ描画
	{
		//player->RenderDebug(rc, shapeRenderer, { 1,2,1 }, { 1,1,1,1 }, DEBUG_MODE::BOX | DEBUG_MODE::CAPSULE);

	}


	/// 当たり判定の更新
	Collision();

	player->UpdateTransform();

	// 2Dスプライト描画
	{
		//minimap->Render(player->GetPosition());
    
		auto easeOutSine = [](float x) -> float
			{
				return sin((x * DirectX::XM_PI) / 2);
			};

		if (tutorial_Flug)
		{
			bool next_navi_vision = false;
			switch (tutorial_Step)
			{
			case 16:
				next_navi_vision = true;
				//「(現在を付け足す)現在、敵の活動時間は残り2分。頑張って逃げましょう！」
				tutorial[11]->Render(rc, 0, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
				break;
			case 15:
				next_navi_vision = true;
				//「【残り時間】最後に制限時間です。この秒数が...」
				tutorial[10]->Render(rc, 0, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
				break;
			case 14:
				//残り時間が表示
				break;
			case 13:
				next_navi_vision = true;
				//「さてと、後は時間まで逃げるだけですね。」
				tutorial[9]->Render(rc, 0, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
				break;
			case 12:
				next_navi_vision = true;
				//「【プレイヤー専用通路】（これだけ扉の画像のある説明用の画像を表示して説明）壁沿いにある、緑色のライトが...」
				tutorial[8]->Render(rc, 0, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
				break;
			case 11:
				next_navi_vision = true;
				//「【バッテリー】このように敵は巡回中に、バッテリーを...」
				tutorial[7]->Render(rc, 0, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
				break;
			case 10:
				next_navi_vision = true;
				//「これは敵が落としていったバッテリー...」
				tutorial[6]->Render(rc, 0, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
				break;
			case 6:
				next_navi_vision = true;
				//「操作方法】マウスで視点を...」
				tutorial[5]->Render(rc, 0, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
				break;
			case 5:
				next_navi_vision = true;
				//「【エネルギーゲージ】敵の視点を見るには、エネルギーを...」
				tutorial[4]->Render(rc, 0, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
				break;
			case 4:
				next_navi_vision = true;
				//「少し、ゲージを消費してしまいましたね。」
				tutorial[3]->Render(rc, 0, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
				break;
			case 2:
				//「【操作方法】右クリックで敵の視点を...」
				tutorial[2]->Render(rc, 0, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
				break;
			case 1:
				next_navi_vision = true;
				//「【マップ】あなたの現在位置は、中央の印で...」
				tutorial[1]->Render(rc, 0, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
				break;
			case 0:
				next_navi_vision=true;
				//「…起動完了。 」
				tutorial[0]->Render(rc, 0, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
				break;
			}
			if (next_navi_vision)
			{
				tutorial[12]->Render(rc, 0, 0, 0, 1280, 720, 0, 1, 1, 1, easeOutSine(button_effect_timer) + 0.5f);
			}
		}
	}

	shadow->Release(dc);

	/// フレームバッファのディアクティベート
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->deactivate(dc);
#if 1
	if (player->GetUseCam())
	{
		//enemy

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
		if (!tutorial_Flug || tutorial_Step >= 4)
		{
			metar->render();
		}
		Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::BloomFinal]->deactivate(dc);

		//Timer
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Timer)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Timer)]->activate(dc);
		Graphics::Instance().bit_block_transfer->blit(dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BloomFinal)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::Timer)].Get());
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Timer)]->deactivate(dc);
		//TemporalNoise
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->activate(dc);

		if (!tutorial_Flug || tutorial_Step >= 14)
		{

			Graphics::Instance().bit_block_transfer->blit(dc,
				Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Timer)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::TemporalNoise)].Get());

		}
		else {

			Graphics::Instance().bit_block_transfer->blit(dc,
				Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BloomFinal)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::TemporalNoise)].Get());
		}
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->deactivate(dc);
		//FilmGrainDustPS
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->activate(dc);
		Graphics::Instance().bit_block_transfer->blit(dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::FilmGrainDust)].Get());
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->deactivate(dc);


		//crt
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);

		Graphics::Instance().bit_block_transfer->blit(dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::FilmGrainDust)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);



		Graphics::Instance().bit_block_transfer->blit(
			dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1


		);



	}
	else//player
	{
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

		minimap->Render(player->GetPosition());
		if (!tutorial_Flug || tutorial_Step >= 4)
		{
			metar->render();
		}
		Graphics::Instance().framebuffers[(int)Graphics::PPShaderType::BloomFinal]->deactivate(dc);


		//Timer
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Timer)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Timer)]->activate(dc);
		Graphics::Instance().bit_block_transfer->blit(dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BloomFinal)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::Timer)].Get());
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Timer)]->deactivate(dc);

		//BreathShake
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BreathShake)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BreathShake)]->activate(dc);
		if (!tutorial_Flug || tutorial_Step >= 14)
		{
			//タイマー表示あり
			Graphics::Instance().bit_block_transfer->blit(dc,
				Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Timer)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::BreathShake)].Get());

		}
		else {
			//タイマー表示なし
			Graphics::Instance().bit_block_transfer->blit(dc,
				Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BloomFinal)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::BreathShake)].Get());



		}

		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BreathShake)]->deactivate(dc);

		//VisionBootDown
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->activate(dc);
		Graphics::Instance().bit_block_transfer->blit(dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BreathShake)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::VisionBootDown)].Get());
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->deactivate(dc);


		//FadeToBlack
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->activate(dc);
		Graphics::Instance().bit_block_transfer->blit(dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::NoSignalFinale)].Get());

		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->deactivate(dc);
		//TVNoiseFade
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->activate(dc);
		Graphics::Instance().bit_block_transfer->blit(dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::TVNoiseFade)].Get());
		//minimap->Render(player->GetPosition());
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->deactivate(dc);



		//crt
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);

		switch (selectTrans)
		{
		case SceneGame::SelectTrans::Clear:
			Graphics::Instance().bit_block_transfer->blit(dc,
				Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());

			break;
		case SceneGame::SelectTrans::GameOver:
			Graphics::Instance().bit_block_transfer->blit(dc,
				Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TVNoiseFade)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());

			break;
		case SceneGame::SelectTrans::cnt:
			break;
		default:
			break;
		}

		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);



		Graphics::Instance().bit_block_transfer->blit(
			dc,
			Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1


		);

	}
#endif
#ifndef DEBUG
		CollisionEditor::Instance().Render(rc, shapeRenderer);

#endif // DEBUG

		um.Render(rc);
}

// GUI描画
void SceneGame::DrawGUI()
{
	minimap->DrawImGui();
	RenderContext rc;

	ImGui::Separator();

	// (ToT)
	ImGui::SliderFloat3("lightDirection", reinterpret_cast<float*>(&lightDirection), -10.0f, +1.0f);
	ImGui::DragFloat("shadowMapDrawRect", &SHADOWMAP_DRAWRECT, 0.1f);


	ImGui::Separator();

	if (ImGui::TreeNode("shadow"))
	{
		//	ImGui::Text("shadow_map");
		shadow->DrawGUI();
		//	//ImGui::Image(shadowShaderResourceView.Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::DragFloat("shadowBias", &shadowBias, 0.0001f, 0, 1, "%.6f");
		ImGui::ColorEdit3("shadowColor", reinterpret_cast<float*>(&shadowColor));
		ImGui::ColorEdit3("edgeColor", &edgeColor.x);

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
	float a = player->GetenableHijackTime();
	ImGui::InputFloat("GetenableHijackTime", &a);
	stage->DrawGUI();
	Graphics::Instance().DebugGUI();
	LightManager::Instance().DebugGUI();
	AirconManager::Instance().DebugGUI();
	ObjectManager::Instance().DebugGUI();

	player->DrawDebug();

	CollisionEditor::Instance().DrawDebug();

	um.DrawDebug();
}

void SceneGame::Collision()
{
	/// プレイヤーとステージとの当たり判定
	PlayerVsStage();

	/// プレイヤーと敵との当たり判定
	PlayerVsEnemy();
}

void SceneGame::PlayerVsStage()
{
	// ---------- 壁との当たり判定 ----------
	/// 当たり判定処理はEditorと分離するべき
	//DirectX::XMFLOAT3 outPos = {};
	//if (CollisionEditor::Instance().Collision(player->GetPosition(), player->GetRadius(), outPos))
	//	player->SetPosition(outPos);

	//DirectX::XMFLOAT3 outPos = {};
	//DirectX::XMFLOAT3 pushDir = {}; // 押し出し方向（法線）


	//if (CollisionEditor::Instance().Collision(player->GetPosition(), player->GetRadius(), outPos, pushDir)) {
	//	// 押し出し方向を正規化
	//	DirectX::XMVECTOR pushVec = DirectX::XMLoadFloat3(&pushDir);
	//	pushVec = DirectX::XMVector3Normalize(pushVec);

	//	// 押し出し距離を調整（例：距離を可変にする）
	//	const float pushDistance = 0.05f; // 小さめにしてがたつきを軽減

	//	DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&outPos);
	//	DirectX::XMVECTOR finalPos = DirectX::XMVectorAdd(posVec, DirectX::XMVectorScale(pushVec, pushDistance));

	//	DirectX::XMFLOAT3 newPos;
	//	DirectX::XMStoreFloat3(&newPos, finalPos);
	//	player->SetPosition(newPos);
	//}



	// ---------- 一方通行通路との当たり判定 ----------

	/// ドア
	DirectX::XMFLOAT3 rayS, rayE;
	rayS = player->GetPosition();
	DirectX::XMFLOAT3 playerDir = player->GetDirection();
	DirectX::XMVECTOR Dir = DirectX::XMLoadFloat3(&playerDir);
	Dir = DirectX::XMVectorScale(Dir, 1.5f);
	DirectX::XMStoreFloat3(&rayE, Dir);

	bool flag = false;
	DirectX::XMFLOAT3 hitPos, hitN;
	for (int i = 0; i < 3; ++i)
	{
		DirectX::XMFLOAT3 stagePos = { stage->GetGateWorld(i).m[3][0], stage->GetGateWorld(i).m[3][1], stage->GetGateWorld(i).m[3][2] };
		stagePos.y += 0.5;
		if (Collision::IntersectSphereVsSphere(player->GetPosition(), player->GetRadius(), stagePos, 0.05, hitPos))
		{
			if (Input::Instance().GetMouse().GetButtonDown() & Mouse::BTN_LEFT && !stage->GetGatePassed(i))
			{
				stage->SetGatePassed(i, true);
				fadeStart = true;
				selectDoorIndex = i;
			}

			flag = true;
			player->SetEnableOpenGate(flag);
		}
		else if (!flag)
		{
			player->SetEnableOpenGate(flag);
		}
	}
}

/**
* @brief プレイヤーと敵との当たり判定関数
*
* プレイヤーと敵の当たり判定を球vs球で取り、
* 互いの hitFlag を true にする
* 押し出しはなし
*/
void SceneGame::PlayerVsEnemy()
{
	float pRadius = player->GetRadius();
	float eRadius = enemy->GetRadius();
	DirectX::XMFLOAT3 pPos = player->GetPosition();
	DirectX::XMFLOAT3 ePos = enemy->GetPosition();

	DirectX::XMFLOAT3 outPos = {};
	if (Collision::IntersectSphereVsSphere(pPos, pRadius, ePos, eRadius, outPos))
	{
		//player->SetIsHit(true);
		enemy->SetIsHit(true);
	}
	else
	{
		player->SetIsHit(false);
		enemy->SetIsHit(false);
	}
}

/**
* @brief カメラの更新処理
*
* プレイヤー視点カメラから敵視点カメラに切り換えるやつ
*/
void SceneGame::UpdateCamera(float elapsedTime)
{
	GamePad& gamepad = Input::Instance().GetGamePad();

	/// 一人称 (プレイヤー視点)
	if (typeid(*i_CameraController) == typeid(FPCameraController))
	{
		/// カメラ切り替え　プレイヤー視点 → 敵視点
		bool useCamera = player->GetUseCam();
		POINT screenPoint = { Input::Instance().GetMouse().GetScreenWidth() / 2, Input::Instance().GetMouse().GetScreenHeight() / 2 };
		ClientToScreen(Graphics::Instance().GetWindowHandle(), &screenPoint);

		DirectX::XMFLOAT3 cameraPos = {};
		/// プレイヤー視点
		if (!useCamera)
		{
    	cameraPos = player->GetPosition();
			cameraPos.y = player->GetViewPoint();
			if(!player->GetIsEvent())
			{
				cameraPos = player->GetPosition();
				cameraPos.y = player->GetViewPoint();
			}
			else
			{
				i_CameraController->SetIsEvent(player->GetIsEvent());
				i_CameraController->SetPitch(player->GetPitch());
				i_CameraController->SetYaw(player->GetYaw());
			}
		}
		/// 敵視点
		else
		{
			/// 敵の視点に固定するために
			cameraPos = enemy->GetPosition();
			cameraPos.y = enemy->GetViewPoint();
			i_CameraController->SetPitch(enemy->GetPitch());
			i_CameraController->SetYaw(enemy->GetYaw());
		}

		i_CameraController->SetCameraPos(cameraPos);
		i_CameraController->SetUseEnemyCam(useCamera);
		/// カメラを切り換えた瞬間に元のカメラの角度に戻す処理用
		/// UseEnemyCam とは使用用途が違うので注意
		i_CameraController->SetIsChange(player->GetIsChange());
		i_CameraController->Update(elapsedTime);
		SetCursorPos(screenPoint.x, screenPoint.y);

#ifdef _DEBUG
		/// カメラモードの変更 (DEBUG モードのみ)
		if (gamepad.GetButton() & GamePad::CTRL && gamepad.GetButtonDown() & GamePad::BTN_X)
		{
			//i_CameraController = std::make_unique<FreeCameraController>();
			i_CameraController = std::make_unique<LightDebugCameraController>();
		}
#endif
	}
	else ///< フリーカメラ用
	{
		i_CameraController->Update(elapsedTime);

		if (gamepad.GetButton() & GamePad::CTRL && gamepad.GetButtonDown() & GamePad::BTN_X)
		{
			i_CameraController = std::make_unique<FPCameraController>();
		}
	}
}

/// 一方通行処理
void SceneGame::UpdateOneWay(float elapsedTime)
{
	/// 一方通行の通路を通った時のフェードインフェードアウトの処理
	if (fadeStart)
	{
		static bool flag = false;
		if (!flag)
			fadeTime += elapsedTime;
		else
		{
			fadeTime -= elapsedTime;
		}
		float alpha = fadeTime / totalFadeTime;

		um.GetUIs().at(0)->GetSpriteData().color.w = alpha;

		if (alpha >= 1)
		{
			flag = true;
		}
		else if (alpha <= 1 && alpha > 0.9)
		{
			DirectX::XMFLOAT3 exitPos = stage->GetExitPos(selectDoorIndex);
			player->SetPosition(exitPos);
		}
		else if (alpha < 0 && flag)
		{
			flag = false;
			fadeStart = false;
		}
	}
}

void SceneGame::UpdateConstants(RenderContext& rc)
{
	rc.lightDirection = lightDirection;	// (ToT)+
	// シャドウの設定
	rc.shadowColor = shadowColor;
	rc.shadowBias = shadowBias;
	rc.edgeColor = edgeColor;

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


void SceneGame::TutorialUpdate(float elapsedTime)
{
	if (Input::Instance().GetMouse().GetButtonDown() & Mouse::BTN_LEFT)//左クリックされたら
	{
		tutorial_Step++;
	}

	button_effect_timer += button_effect * elapsedTime;
	if (button_effect_timer > 1.0f)
	{
		button_effect *= -1;
	}
	if (button_effect_timer < 0.0f)
	{
		button_effect *= -1;
		button_effect_timer = 0.0f;
	}

	switch (tutorial_Step)
	{
	case 17:
		tutorial_Flug = false;
		//オートランやらなんやらはここで初期化
		break;
	case 16:
		//「(現在を付け足す)現在、敵の活動時間は残り2分。頑張って逃げましょう！」
		break;
	case 15:
		//「【残り時間】最後に制限時間です。この秒数が...」
		break; 
	case 14:
		//残り時間表示
		tutorialTimer += elapsedTime;
		if (tutorialTimer >= 2.0f)
		{
			tutorial_Step += 1;
			tutorialTimer = 0;
		}
		break;
	case 13:
		//「さてと、後は時間まで逃げるだけですね。」
		break;
	case 12:
		//「【プレイヤー専用通路】（これだけ扉の画像のある説明用の画像を表示して説明）壁沿いにある、緑色のライトが...」
		break;
	case 11:
		//「【バッテリー】このように敵は巡回中に、バッテリーを...」
		break;
	case 10:
		//「これは敵が落としていったバッテリー...」
		break;
	case 9:
		tutorial_Step--;
	case 8:
		batteryManager::Instance().Update(elapsedTime);
		tutorialTimer += elapsedTime;
		if (tutorialTimer >= 2.0f)
		{
			tutorial_Step += 2;
			tutorialTimer = 0;
		}
		break;
	case 7:
		batteryManager::Instance().addBattery({ 0,0,0 });//プレイヤーの見える位置にバッテリーを置く
		tutorialTimer = 0;
		tutorial_Step++;
		break;
	case 6:
		//「【操作方法】マウスで視点を...」
		break;
	case 5:
		//「【エネルギーゲージ】敵の視点を見るには、エネルギーを...」
		break;
	case 4:
		//「少し、ゲージを消費してしまいましたね。」
		break;
	case 3:
		tutorial_Step--;
	case 2:
		//「【操作方法】右クリックで敵の視点を...」
		if (Input::Instance().GetMouse().GetButtonDown() & Mouse::BTN_RIGHT)
		{
			tutorial_Click_Count++;
		}
		if (tutorial_Click_Count >= 2)//右クリックが二回押されたら
		{
			tutorial_Step += 2;
		}
		metar->update(player->GetenableHijackTime());
		break;
	case 1:
		//「【マップ】あなたの現在位置は、中央の印で...」
		break;
	case 0:
		//「…起動完了。 」
		break;
	default:
		break;
	}
}