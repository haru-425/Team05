#include "Collision.h"
#include <DirectXCollision.h>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include <algorithm>
#include <cmath>

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

// ���C�L���X�g
bool Collision::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const DirectX::XMFLOAT4X4& worldTransform,
	const Model* model,
	DirectX::XMFLOAT3& hitPosition,
	DirectX::XMFLOAT3& hitNormal)
{
	bool hit = false;

	// �n�_�ƏI�_���烌�C�̃x�N�g���ƒ��������߂�
	DirectX::XMVECTOR WorldRayStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldRayEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldRayEnd, WorldRayStart);
	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

	float nearestDist = DirectX::XMVectorGetX(WorldRayLength);
	if (nearestDist <= 0.0f) return false;

	DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&worldTransform);

	// ���f�����̑S�Ẵ��b�V���ƌ���������s��
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		//// ���b�V���̃��[���h�s������߂�
		//const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);
		//DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform);
		//DirectX::XMMATRIX WorldTransform = DirectX::XMMatrixMultiply(GlobalTransform, ParentWorldTransform);

        const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);
        if (mesh.nodeIndex >= model->GetNodes().size()) continue;

        DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform);
        DirectX::XMMATRIX WorldTransform = DirectX::XMMatrixMultiply(GlobalTransform, ParentWorldTransform);

        DirectX::XMVECTOR det;
        DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(&det, WorldTransform);
        if (DirectX::XMVectorGetX(det) == 0.0f) continue;

		// ���C�����b�V���̃��[�J����Ԃɕϊ�����
		//DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
		DirectX::XMVECTOR LocalRayStart = DirectX::XMVector3Transform(WorldRayStart, InverseWorldTransform);
		DirectX::XMVECTOR LocalRayEnd = DirectX::XMVector3Transform(WorldRayEnd, InverseWorldTransform);
		DirectX::XMVECTOR LocalRayVec = DirectX::XMVectorSubtract(LocalRayEnd, LocalRayStart);
		DirectX::XMVECTOR LocalRayDirection = DirectX::XMVector3Normalize(LocalRayVec);
		float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(LocalRayVec));

		// ���b�V�����̑S�Ă̎O�p�`�ƌ���������s��
		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			// �O�p�`�̒��_���W���擾
			const ModelResource::Vertex& a = mesh.vertices.at(mesh.indices.at(i + 0));
			const ModelResource::Vertex& b = mesh.vertices.at(mesh.indices.at(i + 1));
			const ModelResource::Vertex& c = mesh.vertices.at(mesh.indices.at(i + 2));

			DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
			DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

			// �O�p�`�̌�������
			if (DirectX::TriangleTests::Intersects(LocalRayStart, LocalRayDirection, A, B, C, distance))
			{
				// ���b�V���̃��[�J����Ԃł̌�_�����߂�
				DirectX::XMVECTOR LocalHitVec = DirectX::XMVectorScale(LocalRayDirection, distance);
				DirectX::XMVECTOR LocalHitPosition = DirectX::XMVectorAdd(LocalRayStart, LocalHitVec);

				// ���b�V���̃��[�J����Ԃł̌�_�����[���h��Ԃɕϊ�����
				DirectX::XMVECTOR WorldHitPosition = DirectX::XMVector3Transform(LocalHitPosition, WorldTransform);

				// ���[���h��Ԃł̃��C�̎n�_�����_�܂ł̋��������߂�
				DirectX::XMVECTOR WorldHitVec = DirectX::XMVectorSubtract(WorldHitPosition, WorldRayStart);
				DirectX::XMVECTOR WorldHitDist = DirectX::XMVector3Length(WorldHitVec);
				float worldHitDist = DirectX::XMVectorGetX(WorldHitDist);

				// ��_�܂ł̋��������܂łɌv�Z������_���߂��ꍇ�͌��������Ɣ��肷��
				if (worldHitDist <= nearestDist)
				{
					// ���b�V���̃��[�J����Ԃł̎O�p�`�̖@���x�N�g�����Z�o
					DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
					DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
					DirectX::XMVECTOR LocalHitNormal = DirectX::XMVector3Cross(AB, BC);

					// ���b�V���̃��[�J����Ԃł̖@���x�N�g�������[���h��Ԃɕϊ�����
					DirectX::XMVECTOR WorldHitNormal = DirectX::XMVector3TransformNormal(LocalHitNormal, WorldTransform);

					// �O�p�`�̗��\����i���ς̌��ʂ��}�C�i�X�Ȃ�Ε\�����j
					DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(LocalRayDirection, LocalHitNormal);
					float dot = DirectX::XMVectorGetX(Dot);
					if (dot < 0)
					{
						// ���������̂Ńq�b�g�����i�[
						DirectX::XMStoreFloat3(&hitNormal, DirectX::XMVector3Normalize(WorldHitNormal));
						DirectX::XMStoreFloat3(&hitPosition, WorldHitPosition);

						// ��_�܂ł̋������X�V����
						nearestDist = worldHitDist;
						hit = true;
					}
				}
			}
		}
	}
	return hit;
}

bool Collision::AABBVsSphere(const DirectX::XMFLOAT3& boxMin, const DirectX::XMFLOAT3& boxMax, const DirectX::XMFLOAT3& sphereCenter, float sphereRadius, DirectX::XMFLOAT3& outPosition)
{
    using namespace DirectX;

    // �ŋߐړ_�����߂�
    XMFLOAT3 closest;
    closest.x = DirectX::XMMax(boxMin.x, DirectX::XMMin(sphereCenter.x, boxMax.x));
    closest.y = DirectX::XMMax(boxMin.y, DirectX::XMMin(sphereCenter.y, boxMax.y));
    closest.z = DirectX::XMMax(boxMin.z, DirectX::XMMin(sphereCenter.z, boxMax.z));

    XMFLOAT3 diff;
    diff.x = sphereCenter.x - closest.x;
    diff.y = sphereCenter.y - closest.y;
    diff.z = sphereCenter.z - closest.z;
    XMVECTOR Diff = XMVector3LengthSq(XMLoadFloat3(&diff));
    float distSq = XMVectorGetX(Diff);
    float radiusSq = sphereRadius * sphereRadius;

    if (distSq < radiusSq) {
        float dist = std::sqrtf(distSq);
        float penetration = sphereRadius - dist;

        // �닗���������
        if (dist != 0.0f) {
            XMFLOAT3 normal;
            normal.x = diff.x / dist; // �P�ʃx�N�g��
            normal.y = diff.y / dist; // �P�ʃx�N�g��
            normal.z = diff.z / dist; // �P�ʃx�N�g��
            outPosition.x = sphereCenter.x + normal.x * penetration; // �����߂�
            outPosition.y = sphereCenter.y + normal.y * penetration; // �����߂�
            outPosition.z = sphereCenter.z + normal.z * penetration; // �����߂�
        }
        else {
            // ���S�ɒ��S��AABB�̒��ɂ��� �� ��ɉ����o���Ƃ����ߑł��ł�OK
            //outPosition.y = sphereCenter.y + sphereRadius;
            //DirectX::XMFLOAT3 normal;
            //normal.x = diff.x / dist; // �P�ʃx�N�g��
            //normal.y = diff.y / dist; // �P�ʃx�N�g��
            //normal.z = diff.z / dist; // �P�ʃx�N�g��
            //outPosition.x =sphereCenter.x + normal.x * 5; // �����߂�
            //outPosition.y =sphereCenter.y + normal.y * 5; // �����߂�
            //outPosition.z =sphereCenter.z + normal.z * 5; // �����߂�
        }

        return true; // �Փˁ������߂��ς�
    }

    return false; // �Փ˂��ĂȂ�
}



bool Collision::AABBVsSphere(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max,
    const DirectX::XMFLOAT3& sphereCenter, float sphereRadius,
    DirectX::XMFLOAT3& outPos, DirectX::XMFLOAT3& pushDir)
{
    // �ŋߐړ_�����߂�
    DirectX::XMFLOAT3 closestPoint = {
        std::max(min.x, std::min(sphereCenter.x, max.x)),
        std::max(min.y, std::min(sphereCenter.y, max.y)),
        std::max(min.z, std::min(sphereCenter.z, max.z))
    };

    // �x�N�g���Ƌ���
    DirectX::XMVECTOR centerVec = DirectX::XMLoadFloat3(&sphereCenter);
    DirectX::XMVECTOR closestVec = DirectX::XMLoadFloat3(&closestPoint);
    DirectX::XMVECTOR dirVec = DirectX::XMVectorSubtract(centerVec, closestVec);
    float dist = DirectX::XMVectorGetX(DirectX::XMVector3Length(dirVec));

    if (dist < sphereRadius)
    {
        // �@���i�����o�������j
        DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(dirVec);
        DirectX::XMVECTOR pushVec = DirectX::XMVectorScale(normal, sphereRadius - dist);
        DirectX::XMVECTOR newPos = DirectX::XMVectorAdd(centerVec, pushVec);

        DirectX::XMStoreFloat3(&outPos, newPos);
        DirectX::XMStoreFloat3(&pushDir, normal);
        return true;
    }

    return false;
}
