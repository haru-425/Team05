#include"System/Graphics.h"
#include"SceneTitle.h"
#include"System/Input.h"
#include"SceneGame.h"
#include"fujimoto.h"
#include"SceneManager.h"
#include"SceneLoading.h"
#include"SceneGraphics.h"
#include "Scene/SceneMattsu.h"
#include "./LightModels/LightManager.h"
#include "Camera/CameraController/SceneCameraController.h"
#include "System/SettingsManager.h"
#include <algorithm>
CONST LONG SHADOWMAP_WIDTH = { 2048 };
CONST LONG SHADOWMAP_HEIGHT = { 2048 };

/// データをロードしてUIの表示を合わせるため
static bool isStart = false;

//初期化
void SceneTitle::Initialize()
{
    //スプライト初期化
    sprite = new Sprite("Data/Sprite/GameTitleStrings.png");
    TitleTimer = 0.25f; // タイトル画面のタイマー初期化
    TitleSignalTimer = 0.0f; // タイトル画面の信号タイマー初期化
    sceneTrans = false; // シーン遷移フラグ初期化

    isStart = true;

    /// ステージ初期化
    {
        /// モデル生成
        model = std::make_unique<Stage>();

        /// 行列作成
        DirectX::XMMATRIX M = DirectX::XMMatrixIdentity();
        scale = { 0.01f, 0.01f, 0.01f };
        DirectX::XMStoreFloat4x4(&world, M);
        UpdateTransform();
    }

    i_cameraController = std::make_unique<SceneCameraController>();

    // shadowMap
    ID3D11Device* device = Graphics::Instance().GetDevice();
    shadow = std::make_unique<ShadowCaster>(device, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);

    LightManager::Instance().Initialize();

    //for (int i = 0; i < 7; ++i)
    //{
    //    uiSprits.emplace_back(std::make_unique<Sprite>("./Data/Sprite/image.png"));
    //} 
    //for (int i = 0; i < 1; ++i)
    //{
    //    ui.emplace_back(std::make_unique<UI>("./Data/Sprite/image.png"));
    //}

    /// ゲーム選択
    um.CreateUI("./Data/Sprite/image.png", "Game");
    um.CreateUI("./Data/Sprite/image.png", "Option");
    um.CreateUI("./Data/Sprite/image.png", "Exit");
    /// オプション項目
    um.CreateUI("./Data/Sprite/back.png", "OptionBack");
    um.CreateUI("./Data/Sprite/image.png" ,"Sensitivity");
    um.CreateUI("./Data/Sprite/volume.png", "Main");
    um.CreateUI("./Data/Sprite/volume.png", "BGM");
    um.CreateUI("./Data/Sprite/volume.png", "SE");
    /// 感度
    um.CreateUI("./Data/Sprite/image.png", "OptionBarBack");
    um.CreateUI("./Data/Sprite/image.png", "OptionBar"); ///< 9
    um.CreateUI("./Data/Sprite/numbers.png", "100");
    um.CreateUI("./Data/Sprite/numbers.png", "10");
    um.CreateUI("./Data/Sprite/numbers.png", "1");
    /// マスター用バー
    um.CreateUI("./Data/Sprite/image.png", "MainBarBack");
    um.CreateUI("./Data/Sprite/image.png", "MainBar"); ///< 14
    um.CreateUI("./Data/Sprite/numbers.png", "Main100");
    um.CreateUI("./Data/Sprite/numbers.png", "Main10");
    um.CreateUI("./Data/Sprite/numbers.png", "Main1");
    /// BGM用バー
    um.CreateUI("./Data/Sprite/image.png", "BGMBarBack");
    um.CreateUI("./Data/Sprite/image.png", "BGMBar"); ///< 19
    um.CreateUI("./Data/Sprite/numbers.png", "BGM100");
    um.CreateUI("./Data/Sprite/numbers.png", "BGM10");
    um.CreateUI("./Data/Sprite/numbers.png", "BGM1");
    /// SE用バー
    um.CreateUI("./Data/Sprite/image.png", "SEBarBack");
    um.CreateUI("./Data/Sprite/image.png", "SEBar"); ///< 24
    um.CreateUI("./Data/Sprite/numbers.png", "SE100");
    um.CreateUI("./Data/Sprite/numbers.png", "SE10");
    um.CreateUI("./Data/Sprite/numbers.png", "SE1");
    /// ゲームモード選択
    um.CreateUI("./Data/Sprite/gameMode.png", "GameMode");
    um.CreateUI("./Data/Sprite/gameMode.png", "Tutorial");
    um.CreateUI("./Data/Sprite/gameMode.png", "Normal");
    um.CreateUI("./Data/Sprite/gameMode.png", "Hard");
    um.CreateUI("./Data/Sprite/gameMode.png", "Info");

	Audio3DSystem::Instance().SetEmitterPositionByTag("atmosphere_noise", Camera::Instance().GetEye());
  // リスナーの初期位置と向きを設定
	Audio3DSystem::Instance().UpdateListener(Camera::Instance().GetEye(), Camera::Instance().GetFront(), Camera::Instance().GetUp());


	// 3Dオーディオシステムの再生開始
	Audio3DSystem::Instance().SetVolumeByAll();
	//Audio3DSystem::Instance().UpdateEmitters();
	Audio3DSystem::Instance().PlayByTag("atmosphere_noise");
	Audio3DSystem::Instance().PlayByTag("aircon");
}

//終了化
void SceneTitle::Finalize()
{
	//スプライト終了化
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
	um.Clear();

	Audio3DSystem::Instance().StopByTag("atmosphere_noise"); // 音声停止
	Audio3DSystem::Instance().StopByTag("aircon"); // 音声停止
}

//更新処理
void SceneTitle::Update(float elapsedTime)
{
	Mouse& mouse = Input::Instance().GetMouse();

	bool isChangeScene = false;
	/// マウス左クリックでメインシーンに遷移
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		isChangeScene = true;
	}

#if 1
	GamePad& gamePad = Input::Instance().GetGamePad();

	// 任意のゲームパッドボタンが押されているか
	const GamePadButton anyButton =
		GamePad::BTN_A
		| GamePad::BTN_B
		| GamePad::BTN_X
		| GamePad::BTN_Y;

	bool buttonPressed = (anyButton & gamePad.GetButton()) != 0;
	bool fKey = GetAsyncKeyState('F') & 0x8000;
	bool mKey = GetAsyncKeyState('M') & 0x8000;
	bool gKey = GetAsyncKeyState('G') & 0x8000;
	bool pKey = GetAsyncKeyState('P') & 0x8000;
	// フラグがまだ立っていない場合に入力検出
	if (!sceneTrans)
	{
		if (isStartGame)
		{
			nextScene = new SceneGame;
			sceneTrans = true;
			TitleSignalTimer = 0.0f; // タイマー再スタート
		}
		else if (fKey)
		{
			nextScene = new fujimoto;
			sceneTrans = true;
			TitleSignalTimer = 0.0f;
		}
		else if (mKey)
		{
			nextScene = new SceneMattsu;
			sceneTrans = true;
			TitleSignalTimer = 0.0f;
		}
		else if (gKey)
		{
			nextScene = new SceneGraphics;
			sceneTrans = true;
			TitleSignalTimer = 0.0f;
		}
		else if (pKey) {
			nextScene = new SceneGame;
			sceneTrans = true;
			TitleSignalTimer = 0.0f;
		}
	}
	else
	{
		// フラグが立っている間タイマーを加算し、1秒以上経ったらシーン切り替え
		TitleSignalTimer += elapsedTime;
		if (TitleSignalTimer >= 1.0f && nextScene != nullptr)
		{
			SceneManager::instance().ChangeScene(new SceneLoading(nextScene));
			nextScene = nullptr; // 多重遷移防止
			sceneTrans = false; // シーン遷移フラグをリセット
		}
	}
#endif

	TitleTimer += elapsedTime;
	Graphics::Instance().UpdateConstantBuffer(TitleTimer, TitleSignalTimer);

	/// カメラ更新処理
	i_cameraController->Update(elapsedTime);

	/// ライト更新処理
	LightManager::Instance().Update();

	UpdateUI();

	Audio3DSystem::Instance().SetEmitterPositionByTag("atmosphere_noise", Camera::Instance().GetEye());
	Audio3DSystem::Instance().UpdateListener(Camera::Instance().GetEye(), Camera::Instance().GetFront(), Camera::Instance().GetUp());
	Audio3DSystem::Instance().UpdateEmitters(elapsedTime);
}


//描画処理
void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();
	Camera& camera = Camera::Instance();
	ModelRenderer* renderer = graphics.GetModelRenderer();

	/// カメラの設定
	camera.SetPerspectiveFov(45, graphics.GetScreenWidth() / graphics.GetScreenHeight(), 0.1f, 1000.0f);

	//描画基準
	RenderContext rc;
	rc.deviceContext = dc;
	rc.lightDirection = lightDirection;
	rc.renderState = graphics.GetRenderState();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	UpdateConstants(rc);
	LightManager::Instance().UpdateConstants(rc);

	/// フレームバッファのクリアとアクティベート（ポストプロセス用）
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->clear(dc, 1, 1, 1, 1);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->activate(dc);

	//全ての効果を掛けるならここ
		// モデルの描画
	{
		//renderer->Render(rc, world, model.get(), ShaderId::Custom);
		model->Render(rc, renderer);

		LightManager::Instance().Render(rc);
	}

	{

	}

#if 1
	// 2Dスプライト描画

#endif

	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->deactivate(dc);
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
	//NoiseChange
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoiseChange)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoiseChange)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::BloomFinal)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::NoiseChange)].Get());
	{
		// タイトル描画
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());

		// スプライトサイズ（実際の描画サイズ）
		float spriteWidth = 2260.0f / 4.0f;
		float spriteHeight = 1077.0f / 4.0f;

		float spritePos[2];
		spritePos[0] = screenWidth / 2.0f - spriteWidth / 2.0f;  // 中央寄せ（X）
		spritePos[1] = 10.0f;  // 上側に固定（Y）
		spritePos[0] -= 350;

		sprite->Render(
			rc,
			spritePos[0], spritePos[1],
			0,
			spriteWidth, spriteHeight,
			0,
			1, 1, 1, 1
		);
	}
	/// UI描画
	um.Render(rc);

	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoiseChange)]->deactivate(dc);
	//ポストプロセス適用
	//Grtch
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoiseChange)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::Glitch)].Get());
	//グリッジを掛けない場合はここ

	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->deactivate(dc);

	//TEMPORAL NOISE
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::TemporalNoise)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->deactivate(dc);
	//LaightFlicker
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::LightFlicker)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::LightFlicker)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::LightFlicker)].Get());

	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::LightFlicker)]->deactivate(dc);

	//crt
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::LightFlicker)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::crt)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->deactivate(dc);

	//NoSignalFinale
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::crt)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::NoSignalFinale)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->deactivate(dc);

	//VisionBootDown
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->clear(dc);
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->activate(dc);
	Graphics::Instance().bit_block_transfer->blit(dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoSignalFinale)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::VisionBootDown)].Get());
	Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->deactivate(dc);

	/// ポストプロセス結果の描画
	Graphics::Instance().bit_block_transfer->blit(
		dc,
		Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->shader_resource_views[0].GetAddressOf(), 10, 1
	);



}

//GUI描画
void SceneTitle::DrawGUI()
{
	LightManager::Instance().DebugGUI();
	i_cameraController->DebugGUI();
	RenderContext rc;

	ImGui::Separator();


	// (ToT)
	ImGui::SliderFloat3("lightDirection", reinterpret_cast<float*>(&lightDirection), -10.0f, +1.0f);
	ImGui::DragFloat("shadowMapDrawRect", &SHADOWMAP_DRAWRECT, 0.1f);

	ImGui::Separator();

	if (ImGui::TreeNode("shadow"))
	{
		//ImGui::Text("shadow_map");
		shadow->DrawGUI();
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
	Graphics::Instance().DebugGUI();


	LightManager::Instance().DebugGUI();

	/// UI更新処理
	um.DrawDebug();
}

void SceneTitle::UpdateTransform()
{
	//スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.x);
	//回転行列
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//３つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;
	//計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&world, W);
}

/// ライトのバッファ更新
void SceneTitle::UpdateConstants(RenderContext& rc)
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

static constexpr float BAR_MIN = 804;  ///< バーの始点
static constexpr float BAR_MAX = 1173; ///< バーの終点
static constexpr float BAR_WIDTH = BAR_MAX - BAR_MIN; ///< バーの長さ
static constexpr float SLIDER_WIDTH = 16;   ///< スライダーの幅
static int sensitivity = 0; ///< 感度
static int mVolume = 0; ///< マスター
static int bgmVolume = 0; ///< bgm
static int seVolume = 0; ///< se
static bool isChangeSettings = false;
void SceneTitle::UpdateUI()
{
    static bool selectOptions = false;
    static bool selectStart = false;
    static bool previousDow = false;
    static int lastSelectID = -1;
    /// 最初シーンを読み込んだときだけ処理が通り
    /// UIを設定に合わせるために設定の値を表示に使う変数に代入
    if (isStart)
    {
        selectOptions = false;
        selectStart = false;
        previousDow = false;
        lastSelectID = -1;

        GameSettings setting = SettingsManager::Instance().GetGameSettings();
        sensitivity = setting.sensitivity  * 100; ///< 感度
        mVolume     = setting.masterVolume * 100; ///< マスター
        bgmVolume   = setting.bgmVolume    * 100; ///< BGM
        seVolume    = setting.seVolume     * 100; ///< SE

        /// スライダーの位置を合わせる                     バーの端からバーの長さを設定値で割ったところがスライダーの位置
        um.GetUIs().at(9)->GetSpriteData().spritePos.x = BAR_MIN + (BAR_WIDTH * setting.sensitivity) - SLIDER_WIDTH;
        um.GetUIs().at(14)->GetSpriteData().spritePos.x = BAR_MIN + (BAR_WIDTH * setting.masterVolume) - SLIDER_WIDTH;
        um.GetUIs().at(19)->GetSpriteData().spritePos.x = BAR_MIN + (BAR_WIDTH * setting.bgmVolume) - SLIDER_WIDTH;
        um.GetUIs().at(24)->GetSpriteData().spritePos.x = BAR_MIN + (BAR_WIDTH * setting.seVolume) - SLIDER_WIDTH;
    }

    Mouse& mouse = Input::Instance().GetMouse();
    SceneManager& sm = SceneManager::instance();

    /// マウス座標取得
    DirectX::XMFLOAT2 mousePos = { (float)Input::Instance().GetMouse().GetPositionX(), (float)Input::Instance().GetMouse().GetPositionY() };
    um.Update(mousePos);

#if 1
	/// ゲーム、設定、終了の三項目の選択を快適にするため
	if (!um.GetUIs().at(0)->GetIsHit() && !um.GetUIs().at(1)->GetIsHit() && !um.GetUIs().at(3)->GetIsHit())
	{
		if (mouse.GetButtonDown() & mouse.BTN_LEFT)
		{
			if (selectOptions)
				selectOptions = false;
			else if (selectStart)
				selectStart = false;
		}
	}
#endif

	/// メニューの選択肢
	for (auto& ui : um.GetUIs())
	{
        int id = ui->GetID();
        if (!(id == 0 || id == 1 || id == 2 || id == 9 ||
            id == 14 || id == 19 || id == 24 || id == 29 ||
            id == 30 || id == 31))continue;


		if (ui->GetIsHit() || (id == 1 && selectOptions) || (id == 0 && selectStart))
		{
			ui->GetSpriteData().color = { 1,1,1,1 };
		}
		else
			ui->GetSpriteData().color = { 0.660,0.660,0.660,1 };
	}

	if (!(mouse.GetButton() & mouse.BTN_LEFT))
	{
		previousDow = false;
		lastSelectID = -1;
	}
	for (auto& ui : um.GetHitAllUI())
	{
		int id = ui->GetID();

		switch (id)
		{
		case 0: ///< id 1はゲーム開始
			if (mouse.GetButtonDown() & mouse.BTN_LEFT)
			{
				selectStart = !selectStart;
				if (selectOptions)
					selectOptions = !selectOptions;

				isStartGame = true;
			}

			break;
		case 1: ///< id 2は設定
			if (mouse.GetButtonDown() & mouse.BTN_LEFT)
			{
				selectOptions = !selectOptions;
				if (selectStart)
					selectStart = !selectStart;
			}
			break;
		case 2: ///< id 2は終了
			if (mouse.GetButtonDown() & mouse.BTN_LEFT)
			{
				/// シーンマネージャーに終了することを伝えて、
				/// Run 関数内で抜けるようにする
				/// exit関数はメモリリークが大量発生する可能性があるのでこの方法にする
				SceneManager::instance().SetIsExit(true);
			}

			break;
		case 9:
			um.GetUIs().at(id)->GetSpriteData().spriteSize = { 20,58 };
			if (mouse.GetButton() & mouse.BTN_LEFT || previousDow)
			{

				previousDow = true;
				lastSelectID = id;
			}
			break;
		case 14:
			um.GetUIs().at(id)->GetSpriteData().spriteSize = { 20,58 };
			if (mouse.GetButton() & mouse.BTN_LEFT || previousDow)
			{

				previousDow = true;
				lastSelectID = id;
			}
			break;
		case 19:
			um.GetUIs().at(id)->GetSpriteData().spriteSize = { 20,58 };
			if (mouse.GetButton() & mouse.BTN_LEFT || previousDow)
			{

				previousDow = true;
				lastSelectID = id;
			}
			break;
		case 24:
			um.GetUIs().at(id)->GetSpriteData().spriteSize = { 20,58 };
			if (mouse.GetButton() & mouse.BTN_LEFT || previousDow)
			{

				previousDow = true;
				lastSelectID = id;
			}
			break;
		case 29:
            if (mouse.GetButtonDown() & mouse.BTN_LEFT)
            {
                sm.SetGameMode(GameMode::Tutorial);
                isStartGame = true;
            }

            um.GetUIs().at(32)->GetSpriteData().texturePos.y = 441;
            um.GetUIs().at(32)->GetSpriteData().spriteSize.y = 86;
            um.GetUIs().at(32)->GetSpriteData().textureSize.y = 92;
            break;
    case 30:
            if (mouse.GetButtonDown() & mouse.BTN_LEFT)
            {
                sm.SetGameMode(GameMode::Noraml);
                isStartGame = true;
            }

            um.GetUIs().at(32)->GetSpriteData().texturePos.y = 539;
            um.GetUIs().at(32)->GetSpriteData().spriteSize.y = 86;
            um.GetUIs().at(32)->GetSpriteData().textureSize.y = 92;
            break;
    case 31:
            if (mouse.GetButtonDown() & mouse.BTN_LEFT)
            {
                sm.SetGameMode(GameMode::Hard);
                isStartGame = true;
            }

            um.GetUIs().at(32)->GetSpriteData().texturePos.y = 610;
            um.GetUIs().at(32)->GetSpriteData().spriteSize.y = 60;
            um.GetUIs().at(32)->GetSpriteData().textureSize.y = 64;
            break;
    default:
            if (!previousDow)
            {
                um.GetUIs().at(9)->GetSpriteData().spriteSize = { 16,54 };
                um.GetUIs().at(14)->GetSpriteData().spriteSize = { 16,54 };
                um.GetUIs().at(19)->GetSpriteData().spriteSize = { 16,54 };
                um.GetUIs().at(24)->GetSpriteData().spriteSize = { 16,54 };
            }

            um.GetUIs().at(32)->GetSpriteData().texturePos.y = -100;
            break;
		  }
	}

#if 1
  /// オプション項目の表示
  if (selectOptions)
  {
      for (int i = 3; i < um.GetUIs().size(); ++i)
      {
          um.GetUIs().at(i)->GetSpriteData().isVisible = true;
      }
  }
  else
  {
      for (int i = 3; i < um.GetUIs().size(); ++i)
      {
          um.GetUIs().at(i)->GetSpriteData().isVisible = false;
      }
  }
  /// ゲームモードの表示
  if (selectStart)
  {
      for (int i = 28; i < um.GetUIs().size(); i++)
      {
          um.GetUIs().at(i)->GetSpriteData().isVisible = true;
      }
  }
  else if(!selectStart)
  {
      for (int i = 28; i < um.GetUIs().size(); i++)
      {
          um.GetUIs().at(i)->GetSpriteData().isVisible = false;
      }
  }
  if (selectStart || selectOptions)
  {
      um.GetUIs().at(3)->GetSpriteData().isVisible = true;
  }
  else if (!selectStart && !selectOptions)
  {
      um.GetUIs().at(3)->GetSpriteData().isVisible = false;
  }
#endif
  
	/// 感度とかのバーの動作
	if (previousDow)
	{
		///
		///                             ↓UIの描画始点
		///                             /--/ <-スライダー
		///                              ↑マウスの座標 = BAR_MIN + SLIDER_WIDTH / 2
		///                             /---------------------------------------------------------------/
		///
		if (lastSelectID == 9)
		{
			mousePos.x = std::clamp((float)mousePos.x, (BAR_MIN + SLIDER_WIDTH / 2), (BAR_MAX - SLIDER_WIDTH / 2));
			sensitivity = um.GetUIs().at(lastSelectID)->GetSpriteData().spritePos.x = mousePos.x - SLIDER_WIDTH / 2;
			sensitivity -= 804.0f; ///< ゲージのUIのX座標
			float barWidth = (BAR_MAX - BAR_MIN) - SLIDER_WIDTH;
			sensitivity /= barWidth / 100;
		}
		else if (lastSelectID == 14)
		{
			mousePos.x = std::clamp((float)mousePos.x, (BAR_MIN + SLIDER_WIDTH / 2), (BAR_MAX - SLIDER_WIDTH / 2));
			mVolume = um.GetUIs().at(lastSelectID)->GetSpriteData().spritePos.x = mousePos.x - SLIDER_WIDTH / 2;
			mVolume -= 804.0f; ///< ゲージのUIのX座標
			float barWidth = (BAR_MAX - BAR_MIN) - SLIDER_WIDTH;
			mVolume /= barWidth / 100;
			//OutputDebugStringA("ID 14 is selected\n");
		}
		else if (lastSelectID == 19)
		{
			mousePos.x = std::clamp((float)mousePos.x, (BAR_MIN + SLIDER_WIDTH / 2), (BAR_MAX - SLIDER_WIDTH / 2));
			bgmVolume = um.GetUIs().at(lastSelectID)->GetSpriteData().spritePos.x = mousePos.x - SLIDER_WIDTH / 2;
			bgmVolume -= 804.0f; ///< ゲージのUIのX座標
			float barWidth = (BAR_MAX - BAR_MIN) - SLIDER_WIDTH;
			bgmVolume /= barWidth / 100;
			//OutputDebugStringA("ID 19 is selected\n");
		}
		else if (lastSelectID == 24)
		{
			mousePos.x = std::clamp((float)mousePos.x, (BAR_MIN + SLIDER_WIDTH / 2), (BAR_MAX - SLIDER_WIDTH / 2));
			seVolume = um.GetUIs().at(lastSelectID)->GetSpriteData().spritePos.x = mousePos.x - SLIDER_WIDTH / 2;
			seVolume -= 804.0f; ///< ゲージのUIのX座標
			float barWidth = (BAR_MAX - BAR_MIN) - SLIDER_WIDTH;
			seVolume /= barWidth / 100;
			//OutputDebugStringA("ID 24 is selected\n");
		}
	}

	/// 100の位
	float hundred[4] = { sensitivity / 100, mVolume / 100, bgmVolume / 100, seVolume / 100 };
	/// 10の位
	float ten[4] = { (sensitivity / 10) % 10, (mVolume / 10) % 10, (bgmVolume / 10) % 10, (seVolume / 10) % 10 };
	/// 1の位
	float one[4] = { sensitivity % 10 , mVolume % 10, bgmVolume % 10, seVolume % 10 };
	/// 数字 UI の表示処理
	if (selectOptions || isStart)
	{
		if (lastSelectID == 9 || !isStart)
		{
			if (hundred[0] == 1)
			{
				um.GetUIs().at(10)->GetSpriteData().isVisible = true;
			}
			else
				um.GetUIs().at(10)->GetSpriteData().isVisible = false;

			if (ten[0] >= 0)
			{
				um.GetUIs().at(11)->GetSpriteData().isVisible = true;
				um.GetUIs().at(11)->GetSpriteData().texturePos.x = 30 + 25 * ((int)ten[0] - 1);
				if (ten[0] == 0 && sensitivity >= 10)
					um.GetUIs().at(11)->GetSpriteData().texturePos.x = 30 + 25 * 9;
			}
			else
				um.GetUIs().at(12)->GetSpriteData().isVisible = false;

			um.GetUIs().at(12)->GetSpriteData().texturePos.x = 30 + 25 * ((int)one[0] - 1);
			if (one[0] == 0)
				um.GetUIs().at(12)->GetSpriteData().texturePos.x = 30 + 25 * 9;
		}

		if (lastSelectID == 14 || !isStart)
		{
			if (hundred[1] == 1)
			{
				um.GetUIs().at(15)->GetSpriteData().isVisible = true;
			}
			else
				um.GetUIs().at(15)->GetSpriteData().isVisible = false;

			if (ten[1] >= 0)
			{
				um.GetUIs().at(16)->GetSpriteData().isVisible = true;
				um.GetUIs().at(16)->GetSpriteData().texturePos.x = 30 + 25 * ((int)ten[1] - 1);
				if (ten[1] == 0 && mVolume >= 10)
					um.GetUIs().at(16)->GetSpriteData().texturePos.x = 30 + 25 * 9;
			}
			else
				um.GetUIs().at(17)->GetSpriteData().isVisible = false;

			um.GetUIs().at(17)->GetSpriteData().texturePos.x = 30 + 25 * ((int)one[1] - 1);
			if (one[1] == 0)
				um.GetUIs().at(17)->GetSpriteData().texturePos.x = 30 + 25 * 9;
		}

		if (lastSelectID == 19 || !isStart)
		{
			if (hundred[2] == 1)
			{
				um.GetUIs().at(20)->GetSpriteData().isVisible = true;
			}
			else
				um.GetUIs().at(20)->GetSpriteData().isVisible = false;

			if (ten[2] >= 0)
			{
				um.GetUIs().at(21)->GetSpriteData().isVisible = true;
				um.GetUIs().at(21)->GetSpriteData().texturePos.x = 30 + 25 * ((int)ten[2] - 1);
				if (ten[2] == 0 && bgmVolume >= 10)
					um.GetUIs().at(21)->GetSpriteData().texturePos.x = 30 + 25 * 9;
			}
			else
				um.GetUIs().at(22)->GetSpriteData().isVisible = false;

			um.GetUIs().at(22)->GetSpriteData().texturePos.x = 30 + 25 * ((int)one[2] - 1);
			if (one[2] == 0)
				um.GetUIs().at(22)->GetSpriteData().texturePos.x = 30 + 25 * 9;
		}

		if (lastSelectID == 24 || !isStart)
		{
			if (hundred[3] == 1)
			{
				um.GetUIs().at(25)->GetSpriteData().isVisible = true;
			}
			else
				um.GetUIs().at(25)->GetSpriteData().isVisible = false;

			if (ten[3] >= 0)
			{
				um.GetUIs().at(26)->GetSpriteData().isVisible = true;
				um.GetUIs().at(26)->GetSpriteData().texturePos.x = 30 + 25 * ((int)ten[3] - 1);
				if (ten[3] == 0 && seVolume >= 10)
					um.GetUIs().at(26)->GetSpriteData().texturePos.x = 30 + 25 * 9;
			}
			else
				um.GetUIs().at(26)->GetSpriteData().isVisible = false;

			um.GetUIs().at(27)->GetSpriteData().texturePos.x = 30 + 25 * ((int)one[3] - 1);
			if (one[3] == 0)
				um.GetUIs().at(27)->GetSpriteData().texturePos.x = 30 + 25 * 9;
		}
	}

	ImGui::Begin("test");
	char buffer[256];
	sprintf_s(buffer, "testValue %d", sensitivity);
	ImGui::Text(buffer);
	ImGui::End();

	if (isStart)isStart = false;

#if 1
	/// 設定を変更した場合保存
	GameSettings setting = SettingsManager::Instance().GetGameSettings();;
	if (!selectOptions)
	{
		if (sensitivity * 0.01 != setting.sensitivity)isChangeSettings = true;
		else if (mVolume * 0.01 != setting.masterVolume)isChangeSettings = true;
		else if (bgmVolume * 0.01 != setting.bgmVolume)isChangeSettings = true;
		else if (seVolume * 0.01 != setting.seVolume)isChangeSettings = true;
	}

	//if (isChangeSettings)
	{
		setting.sensitivity = sensitivity * 0.01;
		setting.masterVolume = mVolume * 0.01;
		setting.bgmVolume = bgmVolume * 0.01;
		setting.seVolume = seVolume * 0.01;

		SettingsManager::Instance().SetGameSettings(setting);
		SettingsManager::Instance().Save();
		isChangeSettings = false;
	}
	Audio3DSystem::Instance().masterVolume = setting.masterVolume;
	Audio3DSystem::Instance().seVolume = setting.seVolume;
	Audio3DSystem::Instance().bgmVolume = setting.bgmVolume;
	Audio3DSystem::Instance().SetVolumeByAll();
#endif
}
