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
#include <algorithm>
CONST LONG SHADOWMAP_WIDTH = { 2048 };
CONST LONG SHADOWMAP_HEIGHT = { 2048 };
//初期化
void SceneTitle::Initialize()
{
    //スプライト初期化
    sprite = new Sprite("Data/Sprite/GameTitleStrings.png");
    TitleTimer = 0.25f; // タイトル画面のタイマー初期化
    TitleSignalTimer = 0.0f; // タイトル画面の信号タイマー初期化
    sceneTrans = false; // シーン遷移フラグ初期化

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

    um.CreateUI("./Data/Sprite/image.png", "Game");
    um.CreateUI("./Data/Sprite/image.png", "Option");
    um.CreateUI("./Data/Sprite/image.png", "Exit");
    um.CreateUI("./Data/Sprite/back.png", "OptionBack");
    um.CreateUI("./Data/Sprite/image.png", "OptionBarBack");
    um.CreateUI("./Data/Sprite/image.png", "OptionBar");
    um.CreateUI("./Data/Sprite/image.png" ,"Sensitivity");
    um.CreateUI("./Data/Sprite/image.png");

	// リスナーの初期位置と向きを設定
	Audio3DSystem::Instance().UpdateListener(Camera::Instance().GetEye(), Camera::Instance().GetFront(), Camera::Instance().GetUp());

	Audio3DSystem::Instance().SetVolumeByTag("atmosphere_noise", 0.4f);
	Audio3DSystem::Instance().SetVolumeByTag("aircon", 1.f);
	// 3Dオーディオシステムの再生開始
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
	i_cameraController->Update(elapsedTime);

	Audio3DSystem::Instance().SetEmitterPositionByTag("atmosphere_noise", Camera::Instance().GetEye());
	Audio3DSystem::Instance().UpdateListener(Camera::Instance().GetEye(), Camera::Instance().GetFront(), Camera::Instance().GetUp());
	Audio3DSystem::Instance().UpdateEmitters();
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
    RenderUI(rc);

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

void SceneTitle::RenderUI(const RenderContext& rc)
{
    //for (int i = 0; i < 7; ++i)
    //{
    //    float color[4] = { uiParam[i].color.x, uiParam[i].color.y, uiParam[i].color.z, uiParam[i].color.w };
    //    uiSprits[i]->Render(rc, uiParam[i].position.x, uiParam[i].position.y, uiParam[i].position.z,
    //        uiParam[i].dw, uiParam[i].dh,
    //        uiParam[i].sx, uiParam[i].sy,
    //        uiParam[i].sw, uiParam[i].sh,
    //        uiParam[i].angle,
    //        color[0], color[1], color[2], color[3]);
    //}
    //for (auto& uis : ui)
    //{
    //    uis->Render(rc);
    //}
    um.Render(rc);
}

static constexpr float BAR_WIDTH    = 1173; ///< バーの長さ
static constexpr float BAR_MIN      = 804;  ///< バーの始点
static constexpr float BAR_MAX      = 1173; ///< バーの終点
static constexpr float SLIDER_WIDTH = 16;   ///< スライダーの幅
static int num = 0;
void SceneTitle::UpdateUI()
{
    Mouse& mouse = Input::Instance().GetMouse();

    /// マウス座標取得
    DirectX::XMFLOAT2 mousePos = { (float)Input::Instance().GetMouse().GetPositionX(), (float)Input::Instance().GetMouse().GetPositionY() };
    um.Update(mousePos);

    /// メニューの選択肢
    for (auto& ui : um.GetUIs())
    {
        if (ui->GetID() > 2)break;

        if (ui->GetIsHit())
        {
            ui->GetSpriteData().color = { 1,1,1,1 };
        }
        else
            ui->GetSpriteData().color = { 0.660,0.660,0.660,1 };
    }

    static bool selectOptions = false;
    static bool previousDow = false;
    static int lastSelectID = -1;
    if (!(mouse.GetButton() & mouse.BTN_LEFT))
    {
        previousDow = false;
        lastSelectID = -1;
    }
    for (auto& ui : um.GetHitAllUI())
    {
        int id = ui->GetID();
        if (id > 3 && id < 5)continue;

        switch (id)
        {
        case 0: ///< id 1はゲーム開始
            if (mouse.GetButtonDown() & mouse.BTN_LEFT)
                isStartGame = true;
            break;
        case 1: ///< id 2は設定
            if (mouse.GetButtonDown() & mouse.BTN_LEFT)
                selectOptions = !selectOptions;

            /// オプション項目の表示
            if (selectOptions)
            {
                for (int i = 3; i < um.GetUIs().size(); ++i)
                {
                    um.GetUIs().at(i)->GetSpriteData().isVisible = true;
                }
            }

            break;
        case 2: ///< id 2は終了
            if (mouse.GetButtonDown() & mouse.BTN_LEFT)
                exit(0);
            break;
        case 5:
            if (mouse.GetButton() & mouse.BTN_LEFT || previousDow)
            {
                //um.GetUIs().at(id)->GetSpriteData().spritePos.x = mousePos.x - 10;
                previousDow = true;
                lastSelectID = id;
            }
            break;
        default:
            break;
        }
    }

    /// 感度とかのバーの動作
    if (previousDow)
    {
        ///                                       
        ///                             ↓UIの描画始点
        ///                             /--/ <-スライダー
        ///                              ↑マウスの座標 = BAR_MIN + SLIDER_WIDTH / 2
        ///                             /---------------------------------------------------------------/
        /// 
        /// 
        mousePos.x = std::clamp((float)mousePos.x, (BAR_MIN + SLIDER_WIDTH / 2), (BAR_MAX - SLIDER_WIDTH / 2));
        num = um.GetUIs().at(lastSelectID)->GetSpriteData().spritePos.x = mousePos.x - SLIDER_WIDTH /2;
        num -= 804.0f; ///< ゲージのUIのX座標
        float barWidth = (BAR_MAX - BAR_MIN) - SLIDER_WIDTH;
        num /= barWidth/100;
    }

    ImGui::Begin("test");
    char buffer[256];
    sprintf_s(buffer, "testValue %d", num);
    ImGui::Text(buffer);
    ImGui::End();
}
