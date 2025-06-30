#include "Enemy.h"

Enemy::Enemy()
{
	model=new Model("Data/Model/Slime/Slime.mdl");

	scale.x = scale.y = scale.z = 0.01f;
	radius = 0.5f;

	position.x = 0;
	position.z = 0;
    position.y = 0;
}

Enemy::~Enemy()
{
}


void Enemy::Update(float elapsedTime)
{
	// ステート毎の更新処理
	switch (state)
	{
	case State::Wander:
		Updatemovement(elapsedTime);
		break;
	case State::Idle:
		//UpdateIdleState(elapsedTime);
		break;
	case State::Attack:
		//UpdateAttackState(elapsedTime);
		break;
	}

	//
	UpdateTransform();

	//
	model->UpdateTransform();


	//UpdateVelocity(elapsedTime);
}

void Enemy::Updatemovement(float elapsedTime)
{

	if (route.empty() || currentTargetIndex >= route.size())
	{
		//待機ステートへ遷移
		//state = State::Idle;
		return;
	}

	// 移動方向ベクトル
	DirectX::XMVECTOR posVec = XMLoadFloat3(&position);
	DirectX::XMVECTOR targetVec = XMLoadFloat3(&targetPosition);
	DirectX::XMVECTOR dir = DirectX::XMVectorSubtract(targetVec, posVec);

	// 移動量を計算
	DirectX::XMVECTOR dirNorm = DirectX::XMVector3Normalize(dir);
	DirectX::XMVECTOR moveVec = DirectX::XMVectorScale(dirNorm, moveSpeed * elapsedTime);

	// 現在位置更新
	posVec = DirectX::XMVectorAdd(posVec, moveVec);
	XMStoreFloat3(&position, posVec);

	// ターゲットに十分近づいたら次のポイントへ
	float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(dir));
	if (distance < 0.1f) { // 誤差で止まらないように閾値
		currentTargetIndex++;
		// 次のターゲット
		targetPosition = route[currentTargetIndex];
	}

	//// 目標地点までXZ平面での距離判定
	//float vx = targetPosition.x - position.x;
	//float vz = targetPosition.z - position.z;
	//float distSq = vx * vx + vz * vz;
	//if (distSq < radius * radius)
	//{
	//	// 待機ステートへ遷移
	//	SetIdleState();
	//}

	//// 目標地点へ移動
	//MoveToTarget(elapsedTime, 1.0f, 1.0f);
	//
	// //プレイヤー索敵
	//if (SearchPlayer())
	//{
	//	// 見つかったら攻撃ステートへ遷移
	//	SetAttackState();
	//}
}

void Enemy::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, world, model, ShaderId::Lambert);
}