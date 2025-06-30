#include "Enemy.h"
#include "Player/Player.h"

Enemy::Enemy()
{
    model = std::make_unique<Model>("./Data/Model/Mr.Incredible/Mr.Incredible.mdl");

    viewPoint = 1.5f;
    scale = { 0.01,0.01,0.01 };
}

Enemy::~Enemy()
{
}

void Enemy::Update(float dt)
{
    using namespace DirectX;
    XMMATRIX M = XMLoadFloat4x4(&world);
    XMVECTOR Forward = M.r[2];

    // XMVectorGetX/Y/Z を使って値を取り出す
    float x = DirectX::XMVectorGetX(Forward);
    float y = DirectX::XMVectorGetY(Forward);
    float z = DirectX::XMVectorGetZ(Forward);

    // pitch（上下角）は y 成分から求める
    pitch = asinf(y);

    // yaw（左右角）は x, z 成分から求める（注意: Xがsin, Zがcos）
    yaw = atan2f(x, z);

    angle.y += 1 * dt;

}

void Enemy::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    renderer->Render(rc, world, model.get(), ShaderId::Lambert);
}

void Enemy::DrawDebug()
{
}
