#pragma once
#include<vector>
#include<set>

#include"Projectile.h"

class ProjectileManager
{
public:
    ProjectileManager();
    ~ProjectileManager();

    void Update(float elapsedTime);
    void Render(const RenderContext& rc, ModelRenderer* renderer);
    void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);
    void Register(Projectile* projectile);
    void Clear();
    int GetProjectileCount()const { return static_cast<int>(projectiles.size()); }
    Projectile* GetProjectile(int index) { return projectiles.at(index); }
    void Remove(Projectile* projectile);

private:
    std::vector<Projectile*>  projectiles;
    std::set<Projectile*>     removes;
};

