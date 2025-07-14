#pragma once
#include <vector>
#include <memory>

#include "System/UI.h"
#include "System/RenderContext.h"

class UIManager
{
public:
    UIManager() = default;
    ~UIManager();

    void CreateUI(const char* filename, const char* saveFilename = nullptr);

    void Update(const DirectX::XMFLOAT2& mousePos);

    void Render(const RenderContext& rc);

    void Clear();

    void DrawDebug();

    std::vector<std::shared_ptr<UI>> GetHitAllUI();

    std::vector<std::shared_ptr<UI>> GetUIs() { return uis; }

private:
    std::vector<std::shared_ptr<UI>> uis;
};