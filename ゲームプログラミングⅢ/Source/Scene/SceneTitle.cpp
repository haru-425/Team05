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
#include "./Object/ObjectManager.h"
#include "./Aircon/AirconManager.h"
#include "System/difficulty.h"
#include "Camera/CameraController/SceneCameraController.h"
#include "System/SettingsManager.h"
#include "System/Audio.h"
#include "System/CursorManager.h"
#include <algorithm>
CONST LONG SHADOWMAP_WIDTH = { 2048 };
CONST LONG SHADOWMAP_HEIGHT = { 2048 };

#define MAX_LIFE 3

/// データをロードしてUIの表示を合わせるため
static bool isStart = false;

//初期化
void SceneTitle::Initialize()
{
    CursorManager::Instance().SetCursorVisible(true);

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

	// ライトの初期化
	LightManager::Instance().Initialize();

	// エアコンの初期化
	AirconManager::Instance().Initialize();

	ObjectManager::Instance().Initialize();

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
	um.CreateUI("./Data/Sprite/image.png", "Sensitivity");
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

	isVolumeSliderActive = false;
	oldSelect = -1;

	Audio3DSystem::Instance().SetEmitterPositionByTag("atmosphere_noise", Camera::Instance().GetEye());
	// リスナーの初期位置と向きを設定
	Audio3DSystem::Instance().UpdateListener(Camera::Instance().GetEye(), Camera::Instance().GetFront(), Camera::Instance().GetUp());


	// 3Dオーディオシステムの再生開始
	Audio3DSystem::Instance().SetVolumeByAll();
	//Audio3DSystem::Instance().UpdateEmitters();
	Audio3DSystem::Instance().PlayByTag("atmosphere_noise");
	Audio3DSystem::Instance().PlayByTag("aircon");

	// SE読み込み
	selectSE = Audio::Instance().LoadAudioSource("Data/Sound/selectButton.wav");

    selectNum = 0;
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

    // SEの終了化
    if (selectSE != nullptr)
    {
        selectSE->Stop();
        delete selectSE;
        selectSE = nullptr;
    }
}

//更新処理
void SceneTitle::Update(float elapsedTime)
{
    Mouse& mouse = Input::Instance().GetMouse();

    GameSettings setting = SettingsManager::Instance().GetGameSettings();
    selectSE->SetVolume(0.5f * setting.seVolume * setting.masterVolume);

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
            nextScene = new SceneGame(MAX_LIFE);
            sceneTrans = true;
            TitleSignalTimer = 0.0f; // タイマー再スタート

            reminingTime = 300.f;

            CursorManager::Instance().SetCursorVisible(false);
        }
        //else if (fKey)
        //{
        //	nextScene = new fujimoto;
        //	sceneTrans = true;

        //	reminingTime = 300.f;
        //	TitleSignalTimer = 0.0f;
        //}
        //else if (mKey)
        //{
        //	nextScene = new SceneMattsu;
        //	sceneTrans = true;
        //	TitleSignalTimer = 0.0f;

        //	reminingTime = 300.f;
        //}
        //else if (gKey)
        //{
        //	nextScene = new SceneGraphics;
        //	sceneTrans = true;
        //	TitleSignalTimer = 0.0f;

        //	reminingTime = 300.f;
        //}
        //else if (pKey) {
        //	nextScene = new SceneGame;
        //	sceneTrans = true;
        //	TitleSignalTimer = 0.0f;

        //	reminingTime = 300.f;
        //}
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

    UpdateUI(elapsedTime);

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

	// shadow
	{
		Camera& camera = Camera::Instance();

		// ライトの位置から見た視線行列を生成
		DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&lightDirection);
		LightPosition = DirectX::XMVectorScale(LightPosition, -50);
		DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(LightPosition,
			DirectX::XMVectorSet(camera.GetFocus().x, camera.GetFocus().y, camera.GetFocus().z, 1.0f),
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
			ObjectManager::Instance().Render(rc, renderer);
		}
		shadow->Deactive(dc);
	}

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

		AirconManager::Instance().Render(rc);

		ObjectManager::Instance().Render(rc, renderer);
	}

	shadow->Release(dc);

#if 1
	// 2Dスプライト描画
	{

	}

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
    //VHS
    Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VHS)]->clear(dc);
    Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VHS)]->activate(dc);
    Graphics::Instance().bit_block_transfer->blit(dc,
        Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::VHS)].Get());
    Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VHS)]->deactivate(dc);
    //RadialBlur
    Graphics::Instance().setRadialBlurCBuffer({ 0.5,0.5 }, 0.0f);

    Graphics::Instance().framebuffers[int(Graphics::PPShaderType::RadialBlur)]->clear(dc);
    Graphics::Instance().framebuffers[int(Graphics::PPShaderType::RadialBlur)]->activate(dc);
    Graphics::Instance().bit_block_transfer->blit(dc,
        Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VHS)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::RadialBlur)].Get());
    Graphics::Instance().framebuffers[int(Graphics::PPShaderType::RadialBlur)]->deactivate(dc);


    //TEMPORAL NOISE
    Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->clear(dc);
    Graphics::Instance().framebuffers[int(Graphics::PPShaderType::TemporalNoise)]->activate(dc);
    Graphics::Instance().bit_block_transfer->blit(dc,
        Graphics::Instance().framebuffers[int(Graphics::PPShaderType::RadialBlur)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::TemporalNoise)].Get());
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
static bool test = false;
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

    ImGui::Checkbox("isVolumeSliderActive", &isVolumeSliderActive);

    Graphics::Instance().DebugGUI();

    LightManager::Instance().DebugGUI();

    /// UI更新処理
    um.DrawDebug();

    DirectX::XMFLOAT2 mousePos;
    mousePos.x = Input::Instance().GetMouse().GetPositionX();
    mousePos.y = Input::Instance().GetMouse().GetPositionY();

    char buf1[256];
    char buf2[256];
    sprintf_s(buf1, sizeof(buf1), "mouseX : %f\n", mousePos.x);
    sprintf_s(buf2, sizeof(buf2), "mouseY : %f\n", mousePos.y);
    ImGui::Begin("mousePos");
    ImGui::Text(buf1);
    ImGui::Text(buf2);

    ImGui::InputInt("selectNum", &selectNum);
    ImGui::Checkbox("selectOption", &test);
    ImGui::End();

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

void SceneTitle::UpdateUI(float elapsedTime)
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
        sensitivity = setting.sensitivity * 100;	///< 感度
        mVolume = setting.masterVolume * 100;	///< マスター
        bgmVolume = setting.bgmVolume * 100;		///< BGM
        seVolume = setting.seVolume * 100;		///< SE

        /// スライダーの位置を合わせる                     バーの端からバーの長さを設定値で割ったところがスライダーの位置
        um.GetUIs().at(9)->GetSpriteData().spritePos.x = BAR_MIN + (BAR_WIDTH * setting.sensitivity) - SLIDER_WIDTH;
        um.GetUIs().at(14)->GetSpriteData().spritePos.x = BAR_MIN + (BAR_WIDTH * setting.masterVolume) - SLIDER_WIDTH;
        um.GetUIs().at(19)->GetSpriteData().spritePos.x = BAR_MIN + (BAR_WIDTH * setting.bgmVolume) - SLIDER_WIDTH;
        um.GetUIs().at(24)->GetSpriteData().spritePos.x = BAR_MIN + (BAR_WIDTH * setting.seVolume) - SLIDER_WIDTH;
    }

    Mouse& mouse = Input::Instance().GetMouse();
    SceneManager& sm = SceneManager::instance();

    /// マウス座標取得
    DirectX::XMFLOAT2 scaleFactor = Graphics::Instance().GetWindowScaleFactor();
    DirectX::XMFLOAT2 mousePos = { (float)Input::Instance().GetMouse().GetPositionX(), (float)Input::Instance().GetMouse().GetPositionY() };
    um.Update(mousePos);

#if 1

    /// ゲーム、設定、終了の三項目の選択を快適にするため
    if (!um.GetUIs().at(0)->GetIsHit() && !um.GetUIs().at(1)->GetIsHit() && !um.GetUIs().at(3)->GetIsHit())
    {
        if (mouse.GetButtonDown() & mouse.BTN_LEFT)
        {
            if (selectOptions) {
                selectOptions = false;
                oldSelect = -1;
                selectSE->Play(false);
            }
            else if (selectStart) {
                selectStart = false;
                oldSelect = -1;
                selectSE->Play(false);
            }
        }
    }
#endif

    /// メニューの選択肢
    for (auto& ui : um.GetUIs())
    {
        int id = ui->GetID();

        // 対象IDのみ処理
        if (!(id == 0 || id == 1 || id == 2 || id == 9 ||
            id == 14 || id == 19 || id == 24 || id == 29 ||
            id == 30 || id == 31)) continue;

        bool isHit = ui->GetIsHit();
        bool isSelected = (id == lastSelectID);
        bool isSpecialID = (id == 9 || id == 14 || id == 19 || id == 24);

        bool shouldHighlight = false;

        // 選択中のときは isSpecialID の中で選択されたUIだけをハイライト
        if (lastSelectID >= 0) {
            if (isSpecialID) {
                shouldHighlight = isSelected;
            }
            else {
                shouldHighlight = (id == 1 && selectOptions) || (id == 0 && selectStart);
            }
        }
        else {
            // 選択されていない場合の通常処理
            if (isSpecialID) {
                if (!isVolumeSliderActive) {
                    shouldHighlight = isHit;
                }
            }
            else {
                shouldHighlight = isHit || (id == 1 && selectOptions) || (id == 0 && selectStart);
            }
        }

        ui->GetSpriteData().color = shouldHighlight ?
            ui->GetSpriteData().color = { 1, 1, 1, 1 } : ui->GetSpriteData().color = { 0.660f, 0.660f, 0.660f, 1 };
    }

    if (!(mouse.GetButton() & mouse.BTN_LEFT))
    {
        previousDow = false;
        lastSelectID = -1;
    }

#pragma region コントローラーでの選択
    GamePad& gamePad = Input::Instance().GetGamePad();
    bool useController = Input::Instance().GetIsGamePadActive();

    if (!selectOptions)
    {
        if (gamePad.GetButtonDown() == GamePad::BTN_DOWN) ++selectNum;
        if (gamePad.GetButtonDown() == GamePad::BTN_UP) --selectNum;
    }
    else
    {
        /// UI の都合上仕方なく
        if (gamePad.GetButtonDown() == GamePad::BTN_DOWN) selectNum += 5;
        if (gamePad.GetButtonDown() == GamePad::BTN_UP) selectNum -= 5;
    }

    if (useController)
    {
        /// Bボタンで戻る処理
        if (selectStart || selectOptions)
        {
            if (selectStart && gamePad.GetButtonDown() & GamePad::BTN_B)
            {
                for (auto& ui : um.GetHitAllUI())
                {
                    ui->SetIsHit(false);
                }
                selectNum = 0;
                selectStart = false;
            }
            else if (selectOptions && gamePad.GetButtonDown() & GamePad::BTN_B)
            {
                for (auto& ui : um.GetHitAllUI())
                {
                    ui->SetIsHit(false);
                }
                selectNum = 1;
                selectOptions = false;
            }
        }

        switch (selectNum)
        {
        case 0: /// ゲーム難易度選択
            um.GetUIs().at(0)->SetIsHit(true);

            if (gamePad.GetButtonDown() & GamePad::BTN_A)
            {
                selectSE->Play(false);

                if (selectOptions)
                    selectOptions = false;

                selectStart = true;
                selectNum = 29;
            }

            um.GetUIs().at(1)->SetIsHit(false);
            um.GetUIs().at(2)->SetIsHit(false);
            break;
        case 1: /// 設定
            um.GetUIs().at(1)->SetIsHit(true);
            if (gamePad.GetButtonDown() & GamePad::BTN_A)
            {
                selectSE->Play(false);

                if (selectStart)
                    selectStart = false;

                selectOptions = true;
                selectNum = 9;
            }

            um.GetUIs().at(0)->SetIsHit(false);
            um.GetUIs().at(2)->SetIsHit(false);
            break;
        case 2: /// ゲーム終了
            um.GetUIs().at(2)->SetIsHit(true);

            if (gamePad.GetButtonDown() & GamePad::BTN_A)
            {
                SceneManager::instance().SetIsExit(true);
            }

            um.GetUIs().at(0)->SetIsHit(false);
            um.GetUIs().at(1)->SetIsHit(false);
            break;
        case 9: /// 感度
            lastSelectID = 9;
            um.GetUIs().at(9)->SetIsHit(true);

            um.GetUIs().at(14)->SetIsHit(false);
            um.GetUIs().at(19)->SetIsHit(false);
            um.GetUIs().at(24)->SetIsHit(false);
            break;
        case 14: /// マスター
            lastSelectID = 14;
            um.GetUIs().at(14)->SetIsHit(true);

            um.GetUIs().at(9)->SetIsHit(false);
            um.GetUIs().at(19)->SetIsHit(false);
            um.GetUIs().at(24)->SetIsHit(false);
            break;
        case 19: /// GBM
            lastSelectID = 19;
            um.GetUIs().at(19)->SetIsHit(true);

            um.GetUIs().at(9)->SetIsHit(false);
            um.GetUIs().at(14)->SetIsHit(false);
            um.GetUIs().at(24)->SetIsHit(false);
            break;
        case 24: /// SE
            lastSelectID = 24;
            um.GetUIs().at(24)->SetIsHit(true);

            um.GetUIs().at(9)->SetIsHit(false);
            um.GetUIs().at(14)->SetIsHit(false);
            um.GetUIs().at(19)->SetIsHit(false);
            break;
        case 29: /// チュートリアル
            /// selectOption選択中にしたボタン押しても9に戻らない原因がこいつで
            /// 24 + 5 = 29でこいつにcaseがいってdefaultに入らないから
            /// ここでオプションのときだけの処理を入れる
            if (selectOptions)
                selectNum = 9;

            um.GetUIs().at(29)->SetIsHit(true);

            um.GetUIs().at(30)->SetIsHit(false);
            um.GetUIs().at(31)->SetIsHit(false);

            if (gamePad.GetButtonDown() & GamePad::BTN_A)
            {
                sm.SetGameMode(GameMode::Tutorial);
                Difficulty::Instance().SetDifficulty(Difficulty::mode::tutorial);
                isStartGame = true;
                selectSE->Play(false);
            }

            um.GetUIs().at(32)->GetSpriteData().texturePos.y = 441;
            um.GetUIs().at(32)->GetSpriteData().spriteSize.y = 86;
            um.GetUIs().at(32)->GetSpriteData().textureSize.y = 92;

            break;
        case 30: /// 普通
            um.GetUIs().at(30)->SetIsHit(true);

            um.GetUIs().at(29)->SetIsHit(false);
            um.GetUIs().at(31)->SetIsHit(false);

            if (gamePad.GetButtonDown() & GamePad::BTN_A)
            {
                sm.SetGameMode(GameMode::Noraml);
                Difficulty::Instance().SetDifficulty(Difficulty::mode::normal);
                isStartGame = true;
                selectSE->Play(false);
            }

            um.GetUIs().at(32)->GetSpriteData().texturePos.y = 539;
            um.GetUIs().at(32)->GetSpriteData().spriteSize.y = 86;
            um.GetUIs().at(32)->GetSpriteData().textureSize.y = 92;

            break;
        case 31: /// むずい
            um.GetUIs().at(31)->SetIsHit(true);

            um.GetUIs().at(29)->SetIsHit(false);
            um.GetUIs().at(30)->SetIsHit(false);

            if (gamePad.GetButtonDown() & GamePad::BTN_A)
            {
                sm.SetGameMode(GameMode::Hard);
                Difficulty::Instance().SetDifficulty(Difficulty::mode::hard);
                isStartGame = true;
                selectSE->Play(false);
            }

            um.GetUIs().at(32)->GetSpriteData().texturePos.y = 610;
            um.GetUIs().at(32)->GetSpriteData().spriteSize.y = 60;
            um.GetUIs().at(32)->GetSpriteData().textureSize.y = 64;

            break;
        default:

            /// ゲーム開始、設定、終了の三項目を選んでいる時
            if (!selectStart && !selectOptions)
            {
                if (selectNum > 2)
                    selectNum = 0;
                else if (selectNum < 0)
                    selectNum = 2;
            }
            else if (selectStart) /// ゲーム開始項目を選んでいる時
            {
                if (selectNum < 29)
                    selectNum = 31;
                else if (selectNum > 31)
                    selectNum = 29;
            }
            else if (selectOptions) /// 設定項目を選んでいる時
            {
                if (selectNum < 9)
                    selectNum = 24;
                else if (selectNum >= 24)
                    selectNum = 9;
            }
            break;
        } /// end switch
    }
    else
    {
        selectNum = -1;
    }

#pragma endregion 

    for (auto& ui : um.GetHitAllUI())
    {
        if (!Input::Instance().GetIsActiveMouse())
            break;

        int id = ui->GetID();

        switch (id)
        {
        case 0: ///< id 0はゲーム開始

            if (mouse.GetButtonDown() & mouse.BTN_LEFT)
            {
                selectStart = true;

                if (selectOptions)
                    selectOptions = !selectOptions;
                if (oldSelect != 0 || selectSE->IsPlaying()) {
                    selectSE->Play(false);
                    oldSelect = 0;
                }
                //isStartGame = true;

            }

            break;
        case 1: ///< id 1は設定
            if (mouse.GetButtonDown() & mouse.BTN_LEFT)
            {
                selectOptions = true;

                if (selectStart)
                    selectStart = !selectStart;
                if (oldSelect != 1 || selectSE->IsPlaying()) {
                    oldSelect = 1;
                    selectSE->Play(false);
                }
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
            if ((mouse.GetButtonDown() & mouse.BTN_LEFT) && um.GetUIs().at(id)->GetIsHit()) {
                um.GetUIs().at(id)->GetSpriteData().spriteSize = { 20,58 };
                isVolumeSliderActive = true;
                lastSelectID = id;
            }
            if (isVolumeSliderActive) {
                if (mouse.GetButton() & mouse.BTN_LEFT || previousDow)
                {
                    previousDow = true;
                    //lastSelectID = id;
                }
            }
            if (mouse.GetButtonUp() & mouse.BTN_LEFT || previousDow) {
                isVolumeSliderActive = false;
            }
            break;
        case 14:
            if ((mouse.GetButtonDown() & mouse.BTN_LEFT) && um.GetUIs().at(id)->GetIsHit()) {
                um.GetUIs().at(id)->GetSpriteData().spriteSize = { 20,58 };
                isVolumeSliderActive = true;
                lastSelectID = id;
            }
            if (isVolumeSliderActive) {
                if (mouse.GetButton() & mouse.BTN_LEFT || previousDow)
                {
                    previousDow = true;
                    //lastSelectID = id;
                }
            }
            if (mouse.GetButtonUp() & mouse.BTN_LEFT || previousDow) {
                isVolumeSliderActive = false;
            }
            break;
        case 19:
            if ((mouse.GetButtonDown() & mouse.BTN_LEFT) && um.GetUIs().at(id)->GetIsHit()) {
                um.GetUIs().at(id)->GetSpriteData().spriteSize = { 20,58 };
                isVolumeSliderActive = true;
                lastSelectID = id;
            }
            if (isVolumeSliderActive) {
                if (mouse.GetButton() & mouse.BTN_LEFT || previousDow)
                {
                    previousDow = true;
                    /*lastSelectID = id;*/
                }
            }
            if (mouse.GetButtonUp() & mouse.BTN_LEFT || previousDow) {
                isVolumeSliderActive = false;
            }
            break;
        case 24:
            if ((mouse.GetButtonDown() & mouse.BTN_LEFT) && um.GetUIs().at(id)->GetIsHit()) {
                um.GetUIs().at(id)->GetSpriteData().spriteSize = { 20,58 };
                isVolumeSliderActive = true;
                lastSelectID = id;
            }
            if (isVolumeSliderActive) {
                if (mouse.GetButton() & mouse.BTN_LEFT || previousDow)
                {
                    previousDow = true;
                    //lastSelectID = id;
                }
            }
            if (mouse.GetButtonUp() & mouse.BTN_LEFT || previousDow) {
                isVolumeSliderActive = false;
            }
            break;
        case 29:
            if (mouse.GetButtonDown() & mouse.BTN_LEFT)
            {
                sm.SetGameMode(GameMode::Tutorial);
                Difficulty::Instance().SetDifficulty(Difficulty::mode::tutorial);
                isStartGame = true;
                selectSE->Play(false);
            }

            um.GetUIs().at(32)->GetSpriteData().texturePos.y = 441;
            um.GetUIs().at(32)->GetSpriteData().spriteSize.y = 86;
            um.GetUIs().at(32)->GetSpriteData().textureSize.y = 92;
            break;
        case 30:
            if (mouse.GetButtonDown() & mouse.BTN_LEFT)
            {
                sm.SetGameMode(GameMode::Noraml);
                Difficulty::Instance().SetDifficulty(Difficulty::mode::normal);
                isStartGame = true;
                selectSE->Play(false);
            }

            um.GetUIs().at(32)->GetSpriteData().texturePos.y = 539;
            um.GetUIs().at(32)->GetSpriteData().spriteSize.y = 86;
            um.GetUIs().at(32)->GetSpriteData().textureSize.y = 92;
            break;
        case 31:
            if (mouse.GetButtonDown() & mouse.BTN_LEFT)
            {
                sm.SetGameMode(GameMode::Hard);
                Difficulty::Instance().SetDifficulty(Difficulty::mode::hard);
                isStartGame = true;
                selectSE->Play(false);
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
    } // end for文

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
    else if (!selectStart)
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
    if (previousDow || useController)
    {
        ///
        ///                             ↓UIの描画始点
        ///                             /--/ <-スライダー
        ///                              ↑マウスの座標 = BAR_MIN + SLIDER_WIDTH / 2
        ///                             /---------------------------------------------------------------/
        ///

        bool isMoveBarR = false;
        bool isMoveBarL = false;
        /// ゲームパッドの設定の値が変化する量の調整
        if (lastSelectID == 9 || lastSelectID == 14 || lastSelectID == 19 || lastSelectID == 24)
        {
            if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_LEFT || Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT)
            {
                selectTime -= elapsedTime;
                if (selectTime <= 0)
                {
                    if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_LEFT)
                        isMoveBarL = true;
                    if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT)
                        isMoveBarR = true;

                    ++selectCount;

                    switch (selectCount)
                    {
                    case 1:
                        selectTime = 0.3f;
                        break;
                    case 2:
                        selectTime = 0.3f;
                        break;
                    case 3:
                        selectTime = 0.2f;
                        break;
                    case 4:
                        selectTime = 0.2f;
                        break;
                    case 5:
                        selectTime = 0.1f;
                        break;
                    case 6:
                        selectTime = 0.1f;
                        break;
                    default:
                        selectTime = 0.03;
                        break;
                    }
                }
            }
            else
            {
                selectTime = 0;
                selectCount = 0;
            }
        }

        /// 全体の長さ
        float valueParHun = (BAR_MAX - SLIDER_WIDTH /2) - (BAR_MIN + SLIDER_WIDTH / 2);
        /// 1でどれだけ進むのか
        valueParHun /= 100;
        if (lastSelectID == 9)
        {
            /// コントローラー使用中
            if (useController)
            {
                if (isMoveBarR)
                    sensitivity++;
                if (isMoveBarL)
                    sensitivity--;

                um.GetUIs().at(9)->GetSpriteData().spritePos.x = BAR_MIN + valueParHun * sensitivity;
                sensitivity = std::clamp(sensitivity, 0, 100);
            }
            else
            {
                mousePos.x = std::clamp(((float)mousePos.x / scaleFactor.x), (BAR_MIN + SLIDER_WIDTH / 2), (BAR_MAX - SLIDER_WIDTH / 2));
                sensitivity = um.GetUIs().at(lastSelectID)->GetSpriteData().spritePos.x = (mousePos.x) - (SLIDER_WIDTH / 2);
                sensitivity -= um.GetUIs().at(8)->GetSpriteData().spritePos.x; ///< ゲージのUIのX座標
                float barWidth = (BAR_MAX - BAR_MIN) - SLIDER_WIDTH;
                sensitivity /= barWidth / 100;
            }
        }
        else if (lastSelectID == 14)
        {
            if (useController)
            {
                if (isMoveBarR)
                    mVolume++;
                if (isMoveBarL)
                    mVolume--;

                um.GetUIs().at(lastSelectID)->GetSpriteData().spritePos.x = BAR_MIN + valueParHun * mVolume;
                mVolume = std::clamp(mVolume, 0, 100);
            }
            else
            {
                mousePos.x = std::clamp(((float)mousePos.x / scaleFactor.x), (BAR_MIN + SLIDER_WIDTH / 2), (BAR_MAX - SLIDER_WIDTH / 2));
                mVolume = um.GetUIs().at(lastSelectID)->GetSpriteData().spritePos.x = mousePos.x - SLIDER_WIDTH / 2;
                mVolume -= um.GetUIs().at(13)->GetSpriteData().spritePos.x; ///< ゲージのUIのX座標
                float barWidth = (BAR_MAX - BAR_MIN) - SLIDER_WIDTH;
                mVolume /= barWidth / 100;
            }
        }
        else if (lastSelectID == 19)
        {
            if (useController)
            {
                if (isMoveBarR)
                    bgmVolume++;
                if (isMoveBarL)
                    bgmVolume--;

                um.GetUIs().at(lastSelectID)->GetSpriteData().spritePos.x = BAR_MIN + valueParHun * bgmVolume;
                bgmVolume = std::clamp(bgmVolume, 0, 100);
            }
            else
            {
                mousePos.x = std::clamp(((float)mousePos.x / scaleFactor.x), (BAR_MIN + SLIDER_WIDTH / 2), (BAR_MAX - SLIDER_WIDTH / 2));
                bgmVolume = um.GetUIs().at(lastSelectID)->GetSpriteData().spritePos.x = mousePos.x - SLIDER_WIDTH / 2;
                bgmVolume -= um.GetUIs().at(18)->GetSpriteData().spritePos.x; ///< ゲージのUIのX座標
                float barWidth = (BAR_MAX - BAR_MIN) - SLIDER_WIDTH;
                bgmVolume /= barWidth / 100;
            }
        }
        else if (lastSelectID == 24)
        {
            if (useController)
            {
                if (isMoveBarR)
                    seVolume++;
                if (isMoveBarL)
                    seVolume--;

                um.GetUIs().at(lastSelectID)->GetSpriteData().spritePos.x = BAR_MIN + valueParHun * seVolume;
                seVolume = std::clamp(seVolume, 0, 100);
            }
            else
            {
                mousePos.x = std::clamp(((float)mousePos.x / scaleFactor.x), (BAR_MIN + SLIDER_WIDTH / 2), (BAR_MAX - SLIDER_WIDTH / 2));
                seVolume = um.GetUIs().at(lastSelectID)->GetSpriteData().spritePos.x = mousePos.x - SLIDER_WIDTH / 2;
                seVolume -= um.GetUIs().at(23)->GetSpriteData().spritePos.x; ///< ゲージのUIのX座標
                float barWidth = (BAR_MAX - BAR_MIN) - SLIDER_WIDTH;
                seVolume /= barWidth / 100;
            }
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

    if (isStart)isStart = false;
    test = selectOptions;
#if 1
    /// 設定を変更した場合保存
    GameSettings setting = SettingsManager::Instance().GetGameSettings();;

    {
        setting.sensitivity = sensitivity * 0.01;
        setting.masterVolume = mVolume * 0.01;
        setting.bgmVolume = bgmVolume * 0.01;
        setting.seVolume = seVolume * 0.01;

        SettingsManager::Instance().SetGameSettings(setting);
        //SettingsManager::Instance().Save();
    }
    Audio3DSystem::Instance().masterVolume = setting.masterVolume;
    Audio3DSystem::Instance().seVolume = setting.seVolume;
    Audio3DSystem::Instance().bgmVolume = setting.bgmVolume;
    Audio3DSystem::Instance().SetVolumeByAll();
#endif
}
