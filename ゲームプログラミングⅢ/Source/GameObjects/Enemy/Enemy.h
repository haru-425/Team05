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

    void SetPlayer(std::shared_ptr<Player> player) { playerRef = player; }

private:
    std::unique_ptr<Model> model;
    std::weak_ptr<Player> playerRef;
};

