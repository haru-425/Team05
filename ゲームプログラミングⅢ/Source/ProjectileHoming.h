#pragma once
#include "Projectile.h"

class ProjectileHoming:public Projectile
{
public:
    ProjectileHoming(ProjectileManager* manager);
    ~ProjectileHoming()override;
    
    // �X�V����
    void Update(float elapsedTime)override;

    void Render(const RenderContext& rc, ModelRenderer* rendererer)override;

    void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target);

private:
    Model* model = nullptr;
    DirectX::XMFLOAT3 target = { 0,0,0 };
    float moveSpeed = 10.0f;
    float turnSpeed = DirectX::XMConvertToRadians(180);
    float lifeTimer = 3.0f;
};


