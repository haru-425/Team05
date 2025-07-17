#include "UiManager.h"
#include "imgui.h"
#include <vector>

UIManager::~UIManager()
{
    if(!uis.empty())
        Clear();
}

static int id = 0;
void UIManager::CreateUI(const char* filename, const char* saveFilename)
{

    uis.emplace_back(std::make_unique<UI>(filename, id, saveFilename));
    ++id;
}

void UIManager::Update(const DirectX::XMFLOAT2& mousePos)
{
    for (auto& ui : uis)
    {
        ui->GetSelectedUI(mousePos);
    }
}

void UIManager::Render(const RenderContext& rc)
{
    for (auto& ui : uis)
    {
        ui->Render(rc);
    }
}

void UIManager::Clear()
{
    uis.clear();
    id = 0;
}

void UIManager::DrawDebug()
{
    if (ImGui::Begin("UIManager"))
    {
        for (auto& ui : uis)
        {
            ui->DrawDebug();
            ImGui::Separator();
        }
    }
    ImGui::End();
}

std::vector<std::shared_ptr<UI>> UIManager::GetHitAllUI()
{
    std::vector<std::shared_ptr<UI>> result;
    for (auto& ui : uis)
    {
        if (!ui->GetIsHit())
        {
            continue;
        }
        else
            result.emplace_back(ui);
    }

    return result;
}
