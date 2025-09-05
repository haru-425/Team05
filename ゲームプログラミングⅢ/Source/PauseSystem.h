#pragma once

#include "System/UI.h"
#include "System/UiManager.h"
#include "System/AudioSource.h"

class PauseSystem
{
public:
    static PauseSystem& Instance()
    {
        static PauseSystem instance;
        return instance;
    }

    PauseSystem() {};
    ~PauseSystem() {};

    void Initialize();
    void Finalize();
    void Update(float elapsedTime);
    void Render(RenderContext& rc);
    void DrawDebug();

    void SetVolume(int lastSelectID, int barID, DirectX::XMFLOAT2 mousePos, int &volume, DirectX::XMFLOAT2 scaleFactor);

    void SetVolumeTexturePosition(int startID, int volume, float texWidght);

private:
    void UpdateWithController(float elapsedTime);

private:
    /// UIスプライト関連
    struct UIParameter
    {
        DirectX::XMFLOAT3 position;
        float dw;
        float dh;
        float angle;
        float sx;
        float sy;
        float sw;
        float sh;
        DirectX::XMFLOAT4 color = { 1,1,1,1 };
    };

    /// UIの情報
    UIParameter uiParam[7] = {};
    std::vector<std::unique_ptr<Sprite>> uiSprits;
    std::vector<std::unique_ptr<UI>> ui;
    UIManager um;
    bool isVolumeSliderActive = false;
    int oldSelect = -1;

    const float BAR_MIN = 754;  ///< バーの始点
    const float BAR_MAX = 1123; ///< バーの終点
    const float BAR_WIDTH = BAR_MAX - BAR_MIN; ///< バーの長さ
    const float SLIDER_WIDTH = 16;   ///< スライダーの幅
    int sensitivity = 0; ///< 感度
    int mVolume = 0; ///< マスター
    int bgmVolume = 0; ///< bgm
    int seVolume = 0; ///< se
    bool isChangeSettings = false;

    // ==============================
    // 音響関連定数
    // ==============================
    AudioSource* selectSE = nullptr;

    bool isSceneStart = false;
    int selectNum = -1;
    float selectTime = 0;
    int selectCount = 0;
};