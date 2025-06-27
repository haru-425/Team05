#pragma once
#include "ICameraController.h"

class FreeCameraController : public ICameraController
{
public:
    FreeCameraController() = default;
    ~FreeCameraController() = default;

    void Update(float dt);

    void DebugGUI() override;
};

