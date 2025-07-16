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

    /// �J�����N���X�ł��ׂ����Ƃł͂Ȃ��C������
    float angler = 10 * 0.01745f;   ///< �J������U�葬�x
    bool isChangeDir = false;       ///< ��U�肪�ς�邩�ǂ���
    float coolTime = 3.0f;          ///< �J�����̎�U��������ς��^�C�~���O
};