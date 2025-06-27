#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>
#include "Edge.h"

class WayPoint {
public:

    // ノードの空間上の位置（3D座標）
    DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    std::string name;
    std::vector<Edge*> edges;
    // 探索済みかどうかのフラグ（trueなら探索済み）
    bool searchFg = false;
    // 開始点からこのノードまでのコスト（初期値は -1.0f で未到達を意味）
    float costFromStart = -1.0f;

    int pointNo;
    // コンストラクタ
    WayPoint() {}
    WayPoint(int no,DirectX::XMFLOAT3 position, const std::string& name = "") : pointNo(no),name(name), position(position) {}

    // デストラクタ（必要に応じてリソース解放）
    ~WayPoint()
    {
        for (auto edge : edges)
        {
            delete edge;
        }
    }

    void AddEdge(WayPoint* destination, float custum_pluss_cost = 0) 
    {
        Edge* edge = new Edge();
        edge->originPoint = pointNo;
        edge->destinationPoint = destination->pointNo;
        DirectX::XMVECTOR cost = DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&destination->position), DirectX::XMLoadFloat3(&this->position)));
        edge->cost = DirectX::XMVectorGetX(cost);
        edge->cost += custum_pluss_cost;
        edges.push_back(edge);
    }

};
