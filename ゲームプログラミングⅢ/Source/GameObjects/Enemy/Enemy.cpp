#include "Enemy.h"
#include "Pursuer/Object.h"
#include "Pursuer/SearchAI.h"
#include "Player/player.h"


Enemy::Enemy(std::shared_ptr<Player> playerRef, Stage* stage)
{
	this->stage = stage;
	this->playerRef = playerRef;
	model=new Model("Data/Model/Slime/Slime.mdl");

	scale.x = scale.y = scale.z = 0.01f;
	radius = 0.5f;

	position.x = 0;
	position.z = 0;
    position.y = 0;

	viewPoint = 1.5;
}

Enemy::~Enemy()
{
}


void Enemy::Update(float elapsedTime)
{
	using namespace DirectX;
#pragma region カメラ用の処理
	XMMATRIX M = XMLoadFloat4x4(&world);
	XMVECTOR Forward = M.r[2];

	float x = XMVectorGetX(Forward);
	float y = XMVectorGetY(Forward);
	float z = XMVectorGetZ(Forward);

	pitch = asinf(y);
	yaw = atan2f(x, z);
#pragma endregion 

	// ステート毎の更新処理
	switch (state)
	{
	case State::Roaming:
		Updatemovement(elapsedTime);
		break;
	case State::Idle:
		Goal::Instance().SetPosition(playerRef.lock()->GetPosition());
		if (GetAsyncKeyState('T') & 0x8000)
		{
			SearchAI::Instance().DijkstraSearch(stage);
			int current = stage->NearWayPointIndex(Goal::Instance().GetPosition());
			int start = stage->NearWayPointIndex(this->GetPosition());

			// ゴールからスタートまで親をたどる
			while (current != start)
			{
				stage->path.push_back(current);
				current = SearchAI::Instance().findRoot[current];
			}
			stage->path.push_back(start);

			// スタート→ゴール順に並べる
			std::reverse(stage->path.begin(), stage->path.end());
			for (auto i : stage->path)
			{
				this->Addroute(stage->wayPoint[i]->position);
			}
			state=State::Roaming;

		}
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
		state = State::Idle;
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

void Enemy::DrawDebug()
{
}

void Enemy::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, world, model, ShaderId::Lambert);
}