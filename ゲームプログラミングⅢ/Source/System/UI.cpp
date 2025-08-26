#include <fstream>
#include <stdio.h>

#include "../External/Json/json.hpp"
#include "imgui.h"

#include "UI.h"
#include "Graphics.h"
#include "System/Input.h"
using json = nlohmann::json;

void to_json(json& j, const SpriteData& data)
{
    j = json{
        {"spritePos", {data.spritePos.x, data.spritePos.y, data.spritePos.z}},
        {"spriteSize", {data.spriteSize.x, data.spriteSize.y}},
        {"texturePos", {data.texturePos.x, data.texturePos.y}},
        {"textureSize", {data.textureSize.x, data.textureSize.y}},
        {"angle", data.angle},
        {"color", {data.color.x, data.color.y, data.color.z, data.color.w}},
        {"isVisible", data.isVisible}
    };
}

void from_json(const json& j, SpriteData& data)
{
    auto pos = j.at("spritePos");
    data.spritePos.x = pos[0].get<float>(); data.spritePos.y = pos[1].get<float>(); data.spritePos.z = pos[2].get<float>();
    auto size = j.at("spriteSize");
    data.spriteSize.x = size[0].get<float>(); data.spriteSize.y = size[1].get<float>();
    auto tPos = j.at("texturePos");
    data.texturePos.x = tPos[0].get<float>(); data.texturePos.y = tPos[1].get<float>();
    auto tSize = j.at("textureSize");
    data.textureSize.x = tSize[0].get<float>(); data.textureSize.y = tSize[1].get<float>();
    j.at("angle").get_to(data.angle);
    auto color = j.at("color");
    data.color.x = color[0].get<float>(); data.color.y = color[1].get<float>(); data.color.z = color[2].get<float>(); data.color.w = color[3].get<float>();

    if (j.contains("isVisible"))
        j.at("isVisible").get_to(data.isVisible);
    else
        data.isVisible = true;
}

UI::UI(const char* filename, int id, const char* saveFilename) : id(id)
{
    sprite = std::make_unique<Sprite>(filename);

    if (!saveFilename)
        this->saveFilename = "None";
    else
        this->saveFilename = saveFilename;

    Load();
}

void UI::Render(const RenderContext& rc)
{
    if (!sprData.isVisible)return;

    sprite->Render(rc, sprData.spritePos.x, sprData.spritePos.y, sprData.spritePos.z,
        sprData.spriteSize.x, sprData.spriteSize.y,
        sprData.texturePos.x, sprData.texturePos.y,
        sprData.textureSize.x, sprData.textureSize.y,
        sprData.angle,
        sprData.color.x, sprData.color.y, sprData.color.z, sprData.color.w);
}

void UI::DrawDebug()
{
    char buffer[256];
    sprintf_s(buffer, "UI_%d", id);

    if (ImGui::TreeNodeEx(buffer, ImGuiTreeNodeFlags_DefaultOpen))
    {
        {
            char buffer[1024];
            ::strncpy_s(buffer, sizeof(buffer), saveFilename.c_str(), sizeof(buffer));
            if (ImGui::InputText("Name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                saveFilename = buffer;
            }
        }

        ImGui::DragFloat2("SpritePos", &sprData.spritePos.x);
        ImGui::DragFloat2("SpriteSize", &sprData.spriteSize.x);
        ImGui::DragFloat2("TexturePos", &sprData.texturePos.x);
        ImGui::DragFloat2("TextureSize", &sprData.textureSize.x);
        ImGui::ColorEdit4("color", &sprData.color.x, ImGuiColorEditFlags_Float);
        ImGui::Checkbox("isVisible", &sprData.isVisible);

        if (ImGui::Button("Save"))
        {
            Save();
        }
        if (ImGui::Button("Load"))
        {
            Load();
        }
        ImGui::TreePop();
    }
}

void UI::GetSelectedUI(const DirectX::XMFLOAT2& mousePos)
{
    /// コントローラーが使われている場合はヒット判定を取らないため
    if (Input::Instance().GetIsGamePadActive())
        return;

    DirectX::XMFLOAT2 scaleFactor = Graphics::Instance().GetWindowScaleFactor();

    if (!sprData.isVisible)
    {
        isHit = false; 
        return;
    }

    if ((sprData.spritePos.x * scaleFactor.x <= mousePos.x && sprData.spritePos.x * scaleFactor.x + sprData.spriteSize.x * scaleFactor.x >= mousePos.x) &&
        (sprData.spritePos.y * scaleFactor.y <= mousePos.y && sprData.spritePos.y * scaleFactor.y + sprData.spriteSize.y * scaleFactor.y >= mousePos.y))
    {
        isHit = true;
#if 0
        /// 当たり判定確認用
        OutputDebugStringA("hit!\n");
#endif
    }
    else
    {
        isHit = false;
    }

}

void UI::Save()
{
    if (strcmp(saveFilename.c_str(), "None") == 0)
    {
        OutputDebugStringA("Please give it a name.\n");
        return;
    }

    std::ofstream ofs("./Data/Settings/UISettings/" + saveFilename + ".json");
    if (!ofs.is_open())
    {
        OutputDebugStringA("can't open file.\n");
        return;
    }
    json j = sprData;
    ofs << j.dump(4);
    ofs.close();
    OutputDebugStringA("Successfully opend the file!!!!!!\n");
}

void UI::Load()
{
    if (strcmp(saveFilename.c_str(), "None") == 0)
    {
        OutputDebugStringA("not have data.\n");
        return;
    }

    std::ifstream ifs("./Data/Settings/UISettings/" + saveFilename + ".json");
    json j;
    if (!ifs.is_open())
    {
        OutputDebugStringA("can't open file.\n");
        return;
    }
    ifs >> j;
    ifs.close();
    OutputDebugStringA("Successfully opend the file!!!!!!\n");

    sprData = j.get<SpriteData>();
}
