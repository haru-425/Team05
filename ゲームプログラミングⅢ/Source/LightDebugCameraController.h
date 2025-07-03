#pragma once

#include "ICameraController.h"

class LightDebugCameraController : public ICameraController
{
public:
    LightDebugCameraController() = default;
    ~LightDebugCameraController() = default;

    void Update(float dt);

    void DebugGUI() override;
};

