#pragma once
#include"System/ModelRenderer.h"
#include"System/ShapeRenderer.h"
//#include"ProjectileManager.h"

// �O���錾
class ProjectileManager;

//�e��
class Projectile
{
public:
    Projectile(ProjectileManager* manager);
    virtual ~Projectile() {};

    virtual void Update(float elapsedTime) = 0;
    virtual void Render(const RenderContext& rc, ModelRenderer* renderer) = 0;
    virtual void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);
    void Destroy();
    const DirectX::XMFLOAT3& GetPosition()const { return position; }
    const DirectX::XMFLOAT3& GetDirection()const { return direction; }
    const DirectX::XMFLOAT3& GetScale()const { return scale; }

    float GetRadius() const { return radius; }

protected:
    void UpdateTransform();

protected:
    DirectX::XMFLOAT3 position = { 0,0,0 };
    DirectX::XMFLOAT3 direction = { 0,0,1 };
    DirectX::XMFLOAT3 scale = { 1,1,1 };
    DirectX::XMFLOAT4X4 transform =
    {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1,
    };

    float radius = 0.5f;

    ProjectileManager* manager = nullptr;
};

