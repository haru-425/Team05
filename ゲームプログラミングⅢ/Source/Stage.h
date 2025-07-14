#pragma once

#include"System/ModelRenderer.h"
#include "Pursuer/WayPoint.h"
#include "Pursuer/Edge.h"
#include "Pursuer/Object.h"
#include "System/LoadTextures.h"

#include <memory>
#include <wrl.h>

#define MAX_WAY_POINT 87

//ステージ
class Stage
{
public:
    Stage();
    ~Stage();

    //ステージ
    void Update(float elapsedTime);

    //描画処理
    void Render(const RenderContext& rc, ModelRenderer* renderer);

    //経路探索用
    // ウェイポイントのインデックスからポジションを取得
    DirectX::XMFLOAT3 GetIndexWayPoint(int index);
    // 指定座標から一番近いウェイポイントのインデックスを取得
    int NearWayPointIndex(DirectX::XMFLOAT3 target);
    // 接続先ポイントを設定
    void DestinationPointSet();

    std::shared_ptr<WayPoint> wayPoint[MAX_WAY_POINT];
    std::vector<int> path;

    DirectX::XMFLOAT4X4 GetWorld() const { return world; }
    Model* GetModel() { return model[0].get(); }

    DirectX::XMFLOAT4X4 GetCollisionMatrix() const { return collisionMeshMatrix; }
    Model* GetCollisionMesh() { return collisionMesh.get(); }

private:
    DirectX::XMFLOAT3 position = { 0,0,0 };
    DirectX::XMFLOAT3 angle = { 0,0,0 };
    DirectX::XMFLOAT3 scale = { 1,1,1 };
    DirectX::XMFLOAT4X4 world = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };

    std::unique_ptr<Model> model[4] = {};

    std::unique_ptr<LoadTextures> textures;
};
