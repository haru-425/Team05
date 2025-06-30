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

    // XMVectorGetX/Y/Z ���g���Ēl�����o��
    float x = DirectX::XMVectorGetX(Forward);
    float y = DirectX::XMVectorGetY(Forward);
    float z = DirectX::XMVectorGetZ(Forward);

    // pitch�i�㉺�p�j�� y �������狁�߂�
    pitch = asinf(y);

    // yaw�i���E�p�j�� x, z �������狁�߂�i����: X��sin, Z��cos�j
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
