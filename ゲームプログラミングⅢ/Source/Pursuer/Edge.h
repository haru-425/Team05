#pragma once
#include <DirectXMath.h>

class WayPoint;

class Edge
{
public:
    // コンストラクタ
    Edge() : cost(0), originPoint(-1), destinationPoint(-1) {}
    // デストラクタ（特に処理なし）
    ~Edge() {}

    int originPoint = -1;       // 辺（エッジ）の始点の頂点番号（初期値は無効な-1）
    int destinationPoint = -1;  // 辺の終点の頂点番号（初期値は無効な-1）
    float defaultcost = 0;
    float cost = 0;             // 辺のコスト（距離や重みなどを表す）
};