#include "ProjectileHoming.h"

ProjectileHoming::ProjectileHoming(ProjectileManager* manager):Projectile(manager)
{
    model = new Model("Data/Model/Sword/Sword.mdl");

    scale.x = scale.y = scale.z = 3.0f;
}

ProjectileHoming::~ProjectileHoming()
{
    delete model;
}

void ProjectileHoming::Update(float elapsedTime)
{
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0.0f)
    {
        Destroy();
    }

    {
        float moveSpeed = this->moveSpeed * elapsedTime;
        position.x += direction.x * moveSpeed;
        position.y += direction.y * moveSpeed;
        position.z += direction.z * moveSpeed;
    }

    {
        float turnSpeed = this->turnSpeed * elapsedTime;

        // ターゲットまでのベクトルを算出
        DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);

        DirectX::XMVECTOR LengthSq = DirectX::XMVector3Length(Vec);
        float lengthSq;
        DirectX::XMStoreFloat(&lengthSq, LengthSq);
        if (lengthSq > 0.00001f)
        {
            Vec = DirectX::XMVector3Normalize(Vec);

            DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);

            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Direction, Vec);

            float dot;
            DirectX::XMStoreFloat(&dot, Dot);

            float rot = 1.0f - dot;
            if (rot > turnSpeed)
            {
                rot = turnSpeed;
            }

            if (fabsf(rot) > 0.0001)
            {
                DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);
                Axis = DirectX::XMVector3Normalize(Axis);

                DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot);

                DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
                Transform = DirectX::XMMatrixMultiply(Transform, Rotation);

                Direction = DirectX::XMVector3Normalize(Transform.r[2]);
                DirectX::XMStoreFloat3(&direction, Direction);
            }
        }
    }
    UpdateTransform();

    model->UpdateTransform();
}

void ProjectileHoming::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    renderer->Render(rc, transform, model, ShaderId::Lambert);
}

void ProjectileHoming::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target)
{
    this->direction = direction;
    this->position = position;
    this->target = target;

    UpdateTransform();
}
