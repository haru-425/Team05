#pragma once
#include "ICameraController.h"

class FPCameraController : public ICameraController
{
public:
    FPCameraController() = default;
    ~FPCameraController() = default;

    void Update(float dt);

    void DebugGUI();
};

