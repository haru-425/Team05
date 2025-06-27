#pragma once
#include "GameObject.h"
#include <memory>
#include "Enemy/Enemy.h"

class Player : public GameObject
{
public:
    Player();
    ~Player();

    // 更新処理
    void Update(float dt) override;

    // 描画処理
    void Render(const RenderContext& rc, ModelRenderer* renderer) override;

    void DrawDebug() override;

    // 敵をバインド
    void SetEnemy(std::shared_ptr<Enemy> enemy) { enemyRef = enemy; }

    bool GetIsChangeCamera() const { return isChangeCamera; }

private:
    void Move(float dt);

    void ChangeCamera();

private:
    std::unique_ptr<Model> model;
    std::shared_ptr<Enemy> enemyRef = nullptr;

    bool isChangeCamera = false;

    DirectX::XMFLOAT3 forward;
};

