#include "Collision.h"

//円同士の当たり判定
bool Collision::IntersectSphereVsSphere(const DirectX::XMFLOAT3& positionA, float radiusA, const DirectX::XMFLOAT3& positionB, float radiusB, DirectX::XMFLOAT3& outPositionB)
{
    //A－Bの単位ベクトルを算出
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec       = DirectX::XMVectorSubtract(PositionB, PositionA);//ベクトルの差を求める
    DirectX::XMVECTOR LengthSq  = DirectX::XMVector3LengthSq(Vec);//ベクトルの長さ √なし
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    //距離判定
    float range = radiusA + radiusB;//半径同士の長さ
    if (range*range<lengthSq)//Sqは二乗の値
    {
        return false;
    }

    //AがBを押し出す
    float Length = DirectX::XMVectorGetX(DirectX::XMVector3Length(Vec));//ベクトルのX成分だけを取る
    float CorrectValue = range - Length;//半径同士の長さーAとBの距離
    //DirectX::XMVECTOR outPosition = DirectX::XMVectorScale(Vec , CorrectValue);
    DirectX::XMVECTOR CorrectVelocity = DirectX::XMVectorScale(DirectX::XMVector3Normalize(Vec), CorrectValue);//補正値とノーマライズしたベクトルを掛ける
    DirectX::XMStoreFloat3(&outPositionB, DirectX::XMVectorAdd(PositionB,CorrectVelocity));//敵のポジションに補正値をたす←押し出し後の敵のポジション

    return true;
}

//円柱同士の当たり判定
bool Collision::IntersectCylinderVsCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outPositionB)
{
    //Aの足元がBの頭より上なら当たっていない
    if (positionA.y > positionB.y + heightB)
    {
        return false;
    }
    //Aの頭がBの足元より下なら当たっていない
    if (positionA.y + heightA < positionB.y)
    {
        return false;
    }
    //XZ平面での範囲チェック
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    float range = radiusA + radiusB;
    float distXZ = sqrtf(vx*vx+vz*vz);
    if (range<distXZ)
    {
            return false;
    }
    //単位ベクトル
    DirectX::XMFLOAT2 zx = {vx,vz};
    DirectX::XMStoreFloat2(&zx, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&zx)));//ベクトルに変換したzxをノーマライズしたものをzxに代入


    //AがBを押し出す
    float correctValue = range - distXZ;
    DirectX::XMVECTOR vector= DirectX::XMVectorScale(DirectX::XMLoadFloat2(&zx), correctValue);
    DirectX::XMFLOAT2 CorrectVelocity;
    DirectX::XMStoreFloat2(&CorrectVelocity,vector);
    outPositionB.x = positionB.x+CorrectVelocity.x;
    outPositionB.y = positionB.y;
    outPositionB.z = positionB.z+CorrectVelocity.y;
    //outPositionB.x = positionA.x + (zx.x * range);
    //outPositionB.y = positionB.y;
    //outPositionB.z = positionA.y + (zx.y * range);

    return true;
}

bool Collision::IntersectSphereVsCylinder(const DirectX::XMFLOAT3& spherePosition, float sphereRadius, const DirectX::XMFLOAT3& cylinderPosition, float cylinderRadius, float cylinderHeight, DirectX::XMFLOAT3& outCylinderPosition)
{
    if (spherePosition.y + sphereRadius < cylinderPosition.y) return false;
    if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight)return false;

    float vx = cylinderPosition.x - spherePosition.x;
    float vz = cylinderPosition.z - spherePosition.z;
    float range = sphereRadius + cylinderRadius;
    float distXZ = sqrtf(vx * vx + vz * vz);
    if (distXZ > range) return false;

    vx /= distXZ;
    vz /= distXZ;
    outCylinderPosition.x = spherePosition.x + (vx * range);
    outCylinderPosition.y = spherePosition.y;
    outCylinderPosition.z = spherePosition.z + (vz * range);

    return false;
}
