#pragma once
#include "System/Model.h"
#include<DirectXMath.h>

//コリジョン
class Collision
{
public:
    //球と球の交差判定
    static bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        DirectX::XMFLOAT3& outPositionB
    );

    //円柱と円柱の交差判定
    static bool IntersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        float heightA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        float heightB,
        DirectX::XMFLOAT3& outPositionB
    );

    static bool IntersectSphereVsCylinder(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereRadius,
        const DirectX::XMFLOAT3& cylinderPosition,
        float cylinderRadius,
        float cylinderHeight,
        DirectX::XMFLOAT3& outCylinderPosition
    );

    
    // レイキャスト
    static bool RayCast(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        const DirectX::XMFLOAT4X4& worldTransform,
        const Model* model,
        DirectX::XMFLOAT3& hitPosition,
        DirectX::XMFLOAT3& hitNormal);

    /**
    * @brief ボックスと球の当たり判定
    * 
    * ボックスと球で当たり判定を行い、
    * 押し出し量も求めます。
    */
    static bool AABBVsSphere(
        const DirectX::XMFLOAT3& boxMin,
        const DirectX::XMFLOAT3& boxMax,
        DirectX::XMFLOAT3& sphereCenter,
        float sphereRadius,
        DirectX::XMFLOAT3& outPosition
    );
};
