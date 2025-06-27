#include "Collision.h"

//�~���m�̓����蔻��
bool Collision::IntersectSphereVsSphere(const DirectX::XMFLOAT3& positionA, float radiusA, const DirectX::XMFLOAT3& positionB, float radiusB, DirectX::XMFLOAT3& outPositionB)
{
    //A�|B�̒P�ʃx�N�g�����Z�o
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec       = DirectX::XMVectorSubtract(PositionB, PositionA);//�x�N�g���̍������߂�
    DirectX::XMVECTOR LengthSq  = DirectX::XMVector3LengthSq(Vec);//�x�N�g���̒��� ��Ȃ�
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    //��������
    float range = radiusA + radiusB;//���a���m�̒���
    if (range*range<lengthSq)//Sq�͓��̒l
    {
        return false;
    }

    //A��B�������o��
    float Length = DirectX::XMVectorGetX(DirectX::XMVector3Length(Vec));//�x�N�g����X�������������
    float CorrectValue = range - Length;//���a���m�̒����[A��B�̋���
    //DirectX::XMVECTOR outPosition = DirectX::XMVectorScale(Vec , CorrectValue);
    DirectX::XMVECTOR CorrectVelocity = DirectX::XMVectorScale(DirectX::XMVector3Normalize(Vec), CorrectValue);//�␳�l�ƃm�[�}���C�Y�����x�N�g�����|����
    DirectX::XMStoreFloat3(&outPositionB, DirectX::XMVectorAdd(PositionB,CorrectVelocity));//�G�̃|�W�V�����ɕ␳�l�������������o����̓G�̃|�W�V����

    return true;
}

//�~�����m�̓����蔻��
bool Collision::IntersectCylinderVsCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outPositionB)
{
    //A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
    if (positionA.y > positionB.y + heightB)
    {
        return false;
    }
    //A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
    if (positionA.y + heightA < positionB.y)
    {
        return false;
    }
    //XZ���ʂł͈̔̓`�F�b�N
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    float range = radiusA + radiusB;
    float distXZ = sqrtf(vx*vx+vz*vz);
    if (range<distXZ)
    {
            return false;
    }
    //�P�ʃx�N�g��
    DirectX::XMFLOAT2 zx = {vx,vz};
    DirectX::XMStoreFloat2(&zx, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&zx)));//�x�N�g���ɕϊ�����zx���m�[�}���C�Y�������̂�zx�ɑ��


    //A��B�������o��
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
