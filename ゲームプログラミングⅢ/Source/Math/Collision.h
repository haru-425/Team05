#pragma once
#include "System/Model.h"
#include<DirectXMath.h>

//�R���W����
class Collision
{
public:
    //���Ƌ��̌�������
    static bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        DirectX::XMFLOAT3& outPositionB
    );

    //�~���Ɖ~���̌�������
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

    
    // ���C�L���X�g
    static bool RayCast(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        const DirectX::XMFLOAT4X4& worldTransform,
        const Model* model,
        DirectX::XMFLOAT3& hitPosition,
        DirectX::XMFLOAT3& hitNormal);

    /**
    * @brief �{�b�N�X�Ƌ��̓����蔻��
    * 
    * �{�b�N�X�Ƌ��œ����蔻����s���A
    * �����o���ʂ����߂܂��B
    */
    static bool AABBVsSphere(
        const DirectX::XMFLOAT3& boxMin,
        const DirectX::XMFLOAT3& boxMax,
        DirectX::XMFLOAT3& sphereCenter,
        float sphereRadius,
        DirectX::XMFLOAT3& outPosition
    );
};
