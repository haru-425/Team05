#pragma once

#include"System/ModelRenderer.h"
#include "Pursuer/WayPoint.h"
#include "Pursuer/Edge.h"
#include "Pursuer/ObjectBase.h"
#include "System/LoadTextures.h"

#include <memory>
#include <wrl.h>

#define MAX_WAY_POINT 87
#define PLAYER_NEAR_DISTANCE 20.0f


//ステージ
class Stage
{
private:
    enum class ModelLavel {
        Aisle = 0,
        CornerCross,
        Floor,
        Room,
        AislePartition,
        Door,
    };
    struct OneWayGate {
        std::shared_ptr<Model> model;              // 扉の3Dモデル
        std::unique_ptr<Model> collisionMesh;
        DirectX::XMFLOAT3      position;
        DirectX::XMFLOAT3      angle;
        bool                   hasPassed = false;          // プレイヤーが通過したかどうか
        DirectX::XMFLOAT4X4    world = {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1
        };
        DirectX::XMFLOAT3 exitPos;
    };

public:
    Stage();
    ~Stage();

    //ステージ
    void Update(float elapsedTime);

    //描画処理
    void Render(const RenderContext& rc, ModelRenderer* renderer);

    void DrawGUI();

    // プレイヤー専用通路用
    DirectX::XMFLOAT4X4 GetGateWorld(int index) const { return gateElements[index].world; }
    Model* GetGateModelCollisionMesh(int index) { return gateElements[index].collisionMesh.get(); }

    void SetGatePassed(int index, bool isUse) { gateElements[index].hasPassed = isUse; }
    bool GetGatePassed(int index) { return gateElements[index].hasPassed; }
    DirectX::XMFLOAT3 GetExitPos(int index) { return gateElements[index].exitPos; }

    //経路探索用
    // ウェイポイントのインデックスからポジションを取得
    DirectX::XMFLOAT3 GetIndexWayPoint(int index);
    // 指定座標から一番近いウェイポイントのインデックスを取得
    int NearWayPointIndex(DirectX::XMFLOAT3 target);
    // 接続先ポイントを設定
    void DestinationPointSet();

    void AddEdgecost(int from, int to);

    int randomPoint();

	void SetPlayerPos(DirectX::XMFLOAT3 pos) { player_pos = pos; }

    std::shared_ptr<WayPoint> wayPoint[MAX_WAY_POINT];
    std::vector<int> path;

    /// ステージの行列
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

    
    static const int MODEL_MAX = 6;

    std::unique_ptr<Model> model[MODEL_MAX] = {};
    std::unique_ptr<LoadTextures> textures[MODEL_MAX] = {};

    OneWayGate gateElements[3];
    std::shared_ptr<Model> gateModelData[2];

    std::unique_ptr<Model> collisionMesh;
    DirectX::XMFLOAT4X4 collisionMeshMatrix;

    DirectX::XMFLOAT3 player_pos;
    std::vector<int> player_NearPoint;
};