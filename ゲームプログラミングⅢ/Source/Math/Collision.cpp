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

// レイキャスト
bool Collision::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const DirectX::XMFLOAT4X4& worldTransform,
	const Model* model,
	DirectX::XMFLOAT3& hitPosition,
	DirectX::XMFLOAT3& hitNormal)
{
	bool hit = false;

	// 始点と終点からレイのベクトルと長さを求める
	DirectX::XMVECTOR WorldRayStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldRayEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldRayEnd, WorldRayStart);
	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

	float nearestDist = DirectX::XMVectorGetX(WorldRayLength);
	if (nearestDist <= 0.0f) return false;

	DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&worldTransform);

	// モデル内の全てのメッシュと交差判定を行う
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		//// メッシュのワールド行列を求める
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

		// レイをメッシュのローカル空間に変換する
		//DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
		DirectX::XMVECTOR LocalRayStart = DirectX::XMVector3Transform(WorldRayStart, InverseWorldTransform);
		DirectX::XMVECTOR LocalRayEnd = DirectX::XMVector3Transform(WorldRayEnd, InverseWorldTransform);
		DirectX::XMVECTOR LocalRayVec = DirectX::XMVectorSubtract(LocalRayEnd, LocalRayStart);
		DirectX::XMVECTOR LocalRayDirection = DirectX::XMVector3Normalize(LocalRayVec);
		float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(LocalRayVec));

		// メッシュ内の全ての三角形と交差判定を行う
		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			// 三角形の頂点座標を取得
			const ModelResource::Vertex& a = mesh.vertices.at(mesh.indices.at(i + 0));
			const ModelResource::Vertex& b = mesh.vertices.at(mesh.indices.at(i + 1));
			const ModelResource::Vertex& c = mesh.vertices.at(mesh.indices.at(i + 2));

			DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
			DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

			// 三角形の交差判定
			if (DirectX::TriangleTests::Intersects(LocalRayStart, LocalRayDirection, A, B, C, distance))
			{
				// メッシュのローカル空間での交点を求める
				DirectX::XMVECTOR LocalHitVec = DirectX::XMVectorScale(LocalRayDirection, distance);
				DirectX::XMVECTOR LocalHitPosition = DirectX::XMVectorAdd(LocalRayStart, LocalHitVec);

				// メッシュのローカル空間での交点をワールド空間に変換する
				DirectX::XMVECTOR WorldHitPosition = DirectX::XMVector3Transform(LocalHitPosition, WorldTransform);

				// ワールド空間でのレイの始点から交点までの距離を求める
				DirectX::XMVECTOR WorldHitVec = DirectX::XMVectorSubtract(WorldHitPosition, WorldRayStart);
				DirectX::XMVECTOR WorldHitDist = DirectX::XMVector3Length(WorldHitVec);
				float worldHitDist = DirectX::XMVectorGetX(WorldHitDist);

				// 交点までの距離が今までに計算した交点より近い場合は交差したと判定する
				if (worldHitDist <= nearestDist)
				{
					// メッシュのローカル空間での三角形の法線ベクトルを算出
					DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
					DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
					DirectX::XMVECTOR LocalHitNormal = DirectX::XMVector3Cross(AB, BC);

					// メッシュのローカル空間での法線ベクトルをワールド空間に変換する
					DirectX::XMVECTOR WorldHitNormal = DirectX::XMVector3TransformNormal(LocalHitNormal, WorldTransform);

					// 三角形の裏表判定（内積の結果がマイナスならば表向き）
					DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(LocalRayDirection, LocalHitNormal);
					float dot = DirectX::XMVectorGetX(Dot);
					if (dot < 0)
					{
						// 交差したのでヒット情報を格納
						DirectX::XMStoreFloat3(&hitNormal, DirectX::XMVector3Normalize(WorldHitNormal));
						DirectX::XMStoreFloat3(&hitPosition, WorldHitPosition);

						// 交点までの距離を更新する
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

    // 最近接点を求める
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

        // 零距離を避ける
        if (dist != 0.0f) {
            XMFLOAT3 normal;
            normal.x = diff.x / dist; // 単位ベクトル
            normal.y = diff.y / dist; // 単位ベクトル
            normal.z = diff.z / dist; // 単位ベクトル
            outPosition.x = sphereCenter.x + normal.x * penetration; // 押し戻し
            outPosition.y = sphereCenter.y + normal.y * penetration; // 押し戻し
            outPosition.z = sphereCenter.z + normal.z * penetration; // 押し戻し
        }
        else {
            // 完全に中心がAABBの中にいる → 上に押し出すとか決め打ちでもOK
            //outPosition.y = sphereCenter.y + sphereRadius;
            //DirectX::XMFLOAT3 normal;
            //normal.x = diff.x / dist; // 単位ベクトル
            //normal.y = diff.y / dist; // 単位ベクトル
            //normal.z = diff.z / dist; // 単位ベクトル
            //outPosition.x =sphereCenter.x + normal.x * 5; // 押し戻し
            //outPosition.y =sphereCenter.y + normal.y * 5; // 押し戻し
            //outPosition.z =sphereCenter.z + normal.z * 5; // 押し戻し
        }

        return true; // 衝突＆押し戻し済み
    }

    return false; // 衝突してない
}



bool Collision::AABBVsSphere(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max,
    const DirectX::XMFLOAT3& sphereCenter, float sphereRadius,
    DirectX::XMFLOAT3& outPos, DirectX::XMFLOAT3& pushDir)
{
    // 最近接点を求める
    DirectX::XMFLOAT3 closestPoint = {
        std::max(min.x, std::min(sphereCenter.x, max.x)),
        std::max(min.y, std::min(sphereCenter.y, max.y)),
        std::max(min.z, std::min(sphereCenter.z, max.z))
    };

    // ベクトルと距離
    DirectX::XMVECTOR centerVec = DirectX::XMLoadFloat3(&sphereCenter);
    DirectX::XMVECTOR closestVec = DirectX::XMLoadFloat3(&closestPoint);
    DirectX::XMVECTOR dirVec = DirectX::XMVectorSubtract(centerVec, closestVec);
    float dist = DirectX::XMVectorGetX(DirectX::XMVector3Length(dirVec));

    if (dist < sphereRadius)
    {
        // 法線（押し出し方向）
        DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(dirVec);
        DirectX::XMVECTOR pushVec = DirectX::XMVectorScale(normal, sphereRadius - dist);
        DirectX::XMVECTOR newPos = DirectX::XMVectorAdd(centerVec, pushVec);

        DirectX::XMStoreFloat3(&outPos, newPos);
        DirectX::XMStoreFloat3(&pushDir, normal);
        return true;
    }

    return false;
}
