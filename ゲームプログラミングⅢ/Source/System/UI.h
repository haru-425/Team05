#pragma once
#include <memory>
#include <string>

#include "System/Input.h"
#include "System/Sprite.h"
#include "System/RenderContext.h"

static struct SpriteData
{
    DirectX::XMFLOAT3 spritePos;
    DirectX::XMFLOAT2 spriteSize;
    DirectX::XMFLOAT2 texturePos;
    DirectX::XMFLOAT2 textureSize;
    float angle = 0;
    DirectX::XMFLOAT4 color = { 0.5,0.5,0.5,1 };
    bool isVisible;
};

class UI
{
public:
    UI(const char* filename, int id, const char* saveFilename = nullptr);
    ~UI() = default;

    void Render(const RenderContext& rc);

    void DrawDebug();

    void GetSelectedUI(const DirectX::XMFLOAT2& mousePos);

    SpriteData& GetSpriteData() { return sprData; }

    bool GetIsHit() const { return isHit; }

    /**
    * @brief スプライトのヒット判定を設定する関数
    * 
    * コントローラーが使用されている場合のみの反応にする
    * UI をもともとコントローラーを使う前提で作ってなかったためにこうなっちゃった
    */
    void SetIsHit(bool isHit) 
    {
        if (Input::Instance().GetIsGamePadActive())
        {
            this->isHit = isHit;
        }
    }

    const std::string& GetSaveFilename() { return saveFilename.c_str(); }

    int GetID() const { return id; }

private:
    void Save();
    void Load();

private:
    std::unique_ptr<Sprite> sprite;
    std::string saveFilename;
    SpriteData sprData;

    DirectX::XMFLOAT2 texturePos;
    DirectX::XMFLOAT2 textureSize;

    bool isHit;

    int id;
};