#pragma once
#include "GameObject.h"
#include <memory>
#include "System/Model.h"

class Player;

class Enemy : public GameObject
{
public:
    Enemy();
    ~Enemy();

    void Update(float dt) override ;

    void Render(const RenderContext& rc, ModelRenderer* renderer);

    void DrawDebug() override;

    void SetPlayer(std::shared_ptr<Player> player) { playerRef = player; }

    float GetPitch() const { return pitch; }
    float GetYaw() const { return yaw; }

private:
    std::unique_ptr<Model> model;
    std::weak_ptr<Player> playerRef;

    float pitch;
    float yaw;
};

