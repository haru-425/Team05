#pragma once
#include "ICameraController.h"

#define VIEW_POINT_COUNT 5

static constexpr float cameraChangeLimit = 5.0f;

class SceneCameraController : public ICameraController
{
public:
    SceneCameraController();
    ~SceneCameraController();

    void Update(float dt) override;

    void DebugGUI()override;

private:
    void SettingViewPointInfo();

private:
    struct ViewPointInfo
    {
        DirectX::XMFLOAT3 target = {};
        DirectX::XMFLOAT3 angle = {};
        float distance = 20;
    };

private:
    float timer = 0.0f;
    int viewPoint = 0;

    ViewPointInfo viewPointInfo[VIEW_POINT_COUNT];

    /// カメラクラスですべきことではない気がする
    float angler = 10 * 0.01745f;   ///< カメラ首振り速度
    bool isChangeDir = false;       ///< 首振りが変わるかどうか
    float coolTime = 3.0f;          ///< カメラの首振り方向が変わるタイミング
};