#pragma once
#include "ICameraController.h"

class FPCameraController : public ICameraController
{
public:
    FPCameraController() = default;
    ~FPCameraController() = default;

    void Update(float dt);

    void DebugGUI();

    static bool GetIsRotating() { return isRotating; }

private:
    inline static bool isRotating = false;
};

