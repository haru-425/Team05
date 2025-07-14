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
//������
void SceneTitle::Initialize()
{
    //�X�v���C�g������
    sprite = new Sprite("Data/Sprite/GameTitleStrings.png");
    TitleTimer = 0.25f; // �^�C�g����ʂ̃^�C�}�[������
    TitleSignalTimer = 0.0f; // �^�C�g����ʂ̐M���^�C�}�[������
    sceneTrans = false; // �V�[���J�ڃt���O������

    /// �X�e�[�W������
    {
        /// ���f������
        model = std::make_unique<Stage>();

        /// �s��쐬
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
}

//�I����
void SceneTitle::Finalize()
{
    //�X�v���C�g�I����
    if (sprite != nullptr)
    {
        delete sprite;
        sprite = nullptr;
    }
    um.Clear();
}

//�X�V����
void SceneTitle::Update(float elapsedTime)
{
    Mouse& mouse = Input::Instance().GetMouse();

    bool isChangeScene = false;
    /// �}�E�X���N���b�N�Ń��C���V�[���ɑJ��
    if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
    {
        isChangeScene = true;
    }

#if 1
    GamePad& gamePad = Input::Instance().GetGamePad();

    // �C�ӂ̃Q�[���p�b�h�{�^����������Ă��邩
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
    // �t���O���܂������Ă��Ȃ��ꍇ�ɓ��͌��o
    if (!sceneTrans)
    {
        if (isStartGame)
        {
            nextScene = new SceneGame;
            sceneTrans = true;
            TitleSignalTimer = 0.0f; // �^�C�}�[�ăX�^�[�g
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
        // �t���O�������Ă���ԃ^�C�}�[�����Z���A1�b�ȏ�o������V�[���؂�ւ�
        TitleSignalTimer += elapsedTime;
        if (TitleSignalTimer >= 1.0f && nextScene != nullptr)
        {
            SceneManager::instance().ChangeScene(new SceneLoading(nextScene));
            nextScene = nullptr; // ���d�J�ږh�~
            sceneTrans = false; // �V�[���J�ڃt���O�����Z�b�g
        }
    }
#endif

    TitleTimer += elapsedTime;
    Graphics::Instance().UpdateConstantBuffer(TitleTimer, TitleSignalTimer);

    /// �J�����X�V����
    i_cameraController->Update(elapsedTime);

    /// ���C�g�X�V����
    LightManager::Instance().Update();

    UpdateUI();
}


//�`�揈��
void SceneTitle::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    RenderState* renderState = graphics.GetRenderState();
    Camera& camera = Camera::Instance();
    ModelRenderer* renderer = graphics.GetModelRenderer();

    /// �J�����̐ݒ�
    camera.SetPerspectiveFov(45, graphics.GetScreenWidth() / graphics.GetScreenHeight(), 0.1f, 1000.0f);

    //�`��
    RenderContext rc;
    rc.deviceContext = dc;
    rc.lightDirection = lightDirection;
    rc.renderState = graphics.GetRenderState();
    rc.view = camera.GetView();
    rc.projection = camera.GetProjection();

    UpdateConstants(rc);
    LightManager::Instance().UpdateConstants(rc);

    /// �t���[���o�b�t�@�̃N���A�ƃA�N�e�B�x�[�g�i�|�X�g�v���Z�X�p�j
    Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->clear(dc, 1, 1, 1, 1);
    Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->activate(dc);

    //�S�Ă̌��ʂ��|����Ȃ炱��
        // ���f���̕`��
    {
        //renderer->Render(rc, world, model.get(), ShaderId::Custom);
        model->Render(rc, renderer);

        LightManager::Instance().Render(rc);
    }

    {

    }

#if 1
    // 2D�X�v���C�g�`��

#endif

    Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->deactivate(dc);
    //NoiseChange
    Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoiseChange)]->clear(dc);
    Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoiseChange)]->activate(dc);
    Graphics::Instance().bit_block_transfer->blit(dc,
        Graphics::Instance().framebuffers[int(Graphics::PPShaderType::screenquad)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::NoiseChange)].Get());
    {
        // �^�C�g���`��
        float screenWidth = static_cast<float>(graphics.GetScreenWidth());
        float screenHeight = static_cast<float>(graphics.GetScreenHeight());

        // �X�v���C�g�T�C�Y�i���ۂ̕`��T�C�Y�j
        float spriteWidth = 2260.0f / 4.0f;
        float spriteHeight = 1077.0f / 4.0f;

        float spritePos[2];
        spritePos[0] = screenWidth / 2.0f - spriteWidth / 2.0f;  // �����񂹁iX�j
        spritePos[1] = 10.0f;  // �㑤�ɌŒ�iY�j
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
    //�|�X�g�v���Z�X�K�p
    //Grtch
    Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->clear(dc);
    Graphics::Instance().framebuffers[int(Graphics::PPShaderType::Glitch)]->activate(dc);
    Graphics::Instance().bit_block_transfer->blit(dc,
        Graphics::Instance().framebuffers[int(Graphics::PPShaderType::NoiseChange)]->shader_resource_views[0].GetAddressOf(), 10, 1, Graphics::Instance().pixel_shaders[int(Graphics::PPShaderType::Glitch)].Get());
    //�O���b�W���|���Ȃ��ꍇ�͂���

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

    /// �|�X�g�v���Z�X���ʂ̕`��
    Graphics::Instance().bit_block_transfer->blit(
        dc,
        Graphics::Instance().framebuffers[int(Graphics::PPShaderType::VisionBootDown)]->shader_resource_views[0].GetAddressOf(), 10, 1
    );


}

//GUI�`��
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

    /// UI�X�V����
    um.DrawDebug();
}

void SceneTitle::UpdateTransform()
{
    //�X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.x);
    //��]�s��
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    //�ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    //�R�̍s���g�ݍ��킹�A���[���h�s����쐬
    DirectX::XMMATRIX W = S * R * T;
    //�v�Z�������[���h�s������o��
    DirectX::XMStoreFloat4x4(&world, W);
}

/// ���C�g�̃o�b�t�@�X�V
void SceneTitle::UpdateConstants(RenderContext& rc)
{
    rc.lightDirection = lightDirection;	// (ToT)+
    // �V���h�E�̐ݒ�
    rc.shadowColor = shadowColor;
    rc.shadowBias = shadowBias;

    // �t�H�O�̐ݒ�
    rc.ambientColor = ambientColor;
    rc.fogColor = fogColor;
    rc.fogRange = fogRange;

    //�J�����p�����[�^�ݒ�
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

static constexpr float BAR_WIDTH    = 1173; ///< �o�[�̒���
static constexpr float BAR_MIN      = 804;  ///< �o�[�̎n�_
static constexpr float BAR_MAX      = 1173; ///< �o�[�̏I�_
static constexpr float SLIDER_WIDTH = 16;   ///< �X���C�_�[�̕�
static int num = 0;
void SceneTitle::UpdateUI()
{
    Mouse& mouse = Input::Instance().GetMouse();

    /// �}�E�X���W�擾
    DirectX::XMFLOAT2 mousePos = { (float)Input::Instance().GetMouse().GetPositionX(), (float)Input::Instance().GetMouse().GetPositionY() };
    um.Update(mousePos);

    /// ���j���[�̑I����
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
        case 0: ///< id 1�̓Q�[���J�n
            if (mouse.GetButtonDown() & mouse.BTN_LEFT)
                isStartGame = true;
            break;
        case 1: ///< id 2�͐ݒ�
            if (mouse.GetButtonDown() & mouse.BTN_LEFT)
                selectOptions = !selectOptions;

            /// �I�v�V�������ڂ̕\��
            if (selectOptions)
            {
                for (int i = 3; i < um.GetUIs().size(); ++i)
                {
                    um.GetUIs().at(i)->GetSpriteData().isVisible = true;
                }
            }

            break;
        case 2: ///< id 2�͏I��
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

    /// ���x�Ƃ��̃o�[�̓���
    if (previousDow)
    {
        ///                                       
        ///                             ��UI�̕`��n�_
        ///                             /--/ <-�X���C�_�[
        ///                              ���}�E�X�̍��W = BAR_MIN + SLIDER_WIDTH / 2
        ///                             /---------------------------------------------------------------/
        /// 
        /// 
        mousePos.x = std::clamp((float)mousePos.x, (BAR_MIN + SLIDER_WIDTH / 2), (BAR_MAX - SLIDER_WIDTH / 2));
        num = um.GetUIs().at(lastSelectID)->GetSpriteData().spritePos.x = mousePos.x - SLIDER_WIDTH /2;
        num -= 804.0f; ///< �Q�[�W��UI��X���W
        float barWidth = (BAR_MAX - BAR_MIN) - SLIDER_WIDTH;
        num /= barWidth/100;
    }

    ImGui::Begin("test");
    char buffer[256];
    sprintf_s(buffer, "testValue %d", num);
    ImGui::Text(buffer);
    ImGui::End();
}
