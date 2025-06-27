#include "Projectile.h"
#include"ProjectileManager.h"

Projectile::Projectile(ProjectileManager* manager):manager(manager)
{
    manager->Register(this);
}
//
//void Projectile::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
//{
//
//}

void Projectile::Destroy()
{
    manager->Remove(this);
}

void Projectile::UpdateTransform()
{
#if 0
    transform._11 = 1.0f * scale.x;
    transform._12 = 0.0f * scale.y;
    transform._13 = 0.0f * scale.z;
    transform._13 = 0.0f;
    transform._21 = 0.0f * scale.y;
    transform._22 = 1.0f * scale.y;
    transform._23 = 0.0f * scale.y;
    transform._24 = 0.0f;
    transform._31 = 0.0f * scale.z;
    transform._32 = 0.0f * scale.z;
    transform._33 = 1.0f * scale.z;
    transform._34 = 0.0f;
    transform._41 = position.x;
    transform._42 = position.y;
    transform._43 = position.z;
    transform._44 = 1.0f;
#endif

    DirectX::XMVECTOR Front, Up, Right;

    //前ベクトルを算出
    Front = DirectX::XMLoadFloat3(&direction);
    Front = DirectX::XMVector2Normalize(Front);

    //仮の上ベクトルを算出
    Up = DirectX::XMVectorSet(0.001f, 1, 0, 0);
    Up = DirectX::XMVector3Normalize(Up);

    //右ベクトルの算出
    Right = DirectX::XMVector3Cross(Up, Front);
    Right = DirectX::XMVector3Normalize(Right);

    //上ベクトルを算出
    Up = DirectX::XMVector3Cross(Front, Right);

    //計算結果を取り出し
    DirectX::XMFLOAT3 front, up, right;
    DirectX::XMStoreFloat3(&front,Front);
    DirectX::XMStoreFloat3(&up, Up);
    DirectX::XMStoreFloat3(&right, Right);

    //算出した軸ベクトルから行列を作成
    transform._11 = right.x * scale.x;
    transform._12 = right.y * scale.x;
    transform._13 = right.z * scale.x;
    transform._13 = 0.0f;
    transform._21 = up.x * scale.y;
    transform._22 = up.y * scale.y;
    transform._23 = up.z * scale.y;
    transform._24 = 0.0f;
    transform._31 = front.x * scale.z;
    transform._32 = front.y * scale.z;
    transform._33 = front.z * scale.z;
    transform._34 = 0.0f;
    transform._41 = position.x;
    transform._42 = position.y;
    transform._43 = position.z;
    transform._44 = 1.0f;

    //発射方向
    this->direction = front;
}

void Projectile::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
    renderer->RenderSphere(rc, position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
}