#include "Enemy.h"
#include <cmath>
#include <iostream>
#include "Pursuer/ObjectBase.h"
#include "Pursuer/SearchAI.h"
#include "Player/player.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneLoading.h"
#include "Scene/SceneGameOver.h"
#include "3DAudio/3dAudio.h"
#include "GameObjects/battery/batteryManager.h"
#include "./Collision.h"
#include "imgui.h"                    // ImGuiの基本機能
#include "imgui_impl_win32.h"        // Win32用バックエンド
#include "imgui_impl_dx11.h"         // DirectX11用バックエンド

// コンストラクタ：プレイヤーとステージへの参照を保持し、モデルを読み込む
Enemy::Enemy(std::shared_ptr<Player> playerRef, Stage* stage)
{
	this->stage = stage;
	this->playerRef = playerRef;
	// モデルの読み込み
	//model = std::make_shared<Model>("Data/Model/Slime/Slime.mdl");
	//model = std::make_shared<Model>("Data/Model/test/enemy_motion.mdl");
	model = std::make_shared<Model>("Data/Model/enemy_assets/enemy_motion.mdl");

	textures = std::make_shared<LoadTextures>();
	textures->LoadNormal("Data/Model/enemy_assets/textures/normal.png");
	textures->LoadRoughness("Data/Model/enemy_assets/textures/roughness.png");
	textures->LoadMetalness("Data/Model/enemy_assets/textures/metalic.png");
	textures->LoadEmisive("Data/Model/enemy_assets/textures/emissive.png");

	this->animationcontroller.SetModel(model);
	this->animationcontroller.SetAnimationPlaying(true);
	scale.x = scale.y = scale.z = 0.013f; // スケール設定（非常に小さい）
	radius = 1.5f;                        // 衝突用の半径
	radius = 1.7f;                        // 衝突用の半径
	viewPoint = 1.5f;                     // 目線の高さ
	position = stage->GetIndexWayPoint(stage->EnemySpawnPoint());      // 初期位置
	state = State::Idle;

	UpdateTransform();
}

Enemy::~Enemy()
{
	// モデルのメモリ解放が必要な場合はここで行う
	Audio3DSystem::Instance().StopByTag("enemy_run");
	Audio3DSystem::Instance().StopByTag("enemy_walk");
}

// 毎フレーム呼び出される更新処理
void Enemy::Update(float elapsedTime)
{
#if 0
	// 敵の向きをプレイヤーに合わせるために、ビュー行列からZ軸方向を取得
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&world);
	DirectX::XMVECTOR Forward = M.r[2];

	float x = DirectX::XMVectorGetX(Forward);
	float y = DirectX::XMVectorGetY(Forward);
	float z = DirectX::XMVectorGetZ(Forward);

#else
	float x, y, z;
	for (int i = 0; i < model->GetNodes().size(); ++i)
	{
		if (std::strcmp(model->GetNodes().at(i).name, "head_end_FK") == 0)
		{
			DirectX::XMFLOAT4X4 transform = model->GetNodes().at(i).globalTransform;
			DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&transform);
			DirectX::XMVECTOR Forward = M.r[2];

			x = DirectX::XMVectorGetX(Forward) * cameraShakeScale.x; ///< 揺れの度合(左右)
			z = DirectX::XMVectorGetZ(Forward) * cameraShakeScale.x; ///< 揺れの度合(左右)

			y = DirectX::XMVectorGetY(Forward) * cameraShakeScale.y; ///< 揺れの度合(上下)
		}
	}
#endif

	pitch = asinf(y);             // 上下の向き
	yaw = atan2f(x, z);           // 左右の向き

	// プレイヤーとのレイキャスト処理
	const DirectX::XMFLOAT3 RayStart = { this->GetPosition().x, this->GetPosition().y + viewPoint, this->GetPosition().z };
	const DirectX::XMFLOAT3 RayGoal = { playerRef.lock()->GetPosition().x, playerRef.lock()->GetPosition().y + viewPoint, playerRef.lock()->GetPosition().z };

	DirectX::XMFLOAT3 hitpos, n;
	//bool a = Collision::RayCast(RayStart, RayGoal, stage->GetWorld(), stage->GetModel(), hitpos, n);      //(デバッグ用)
	loocking = !(Collision::RayCast(RayStart, RayGoal, stage->GetWorld(), stage->GetModel(), hitpos, n));

	/*if (loocking)
	{
		char check[256];
		sprintf_s(check, sizeof(check), "true\n");
		OutputDebugStringA(check);
	}*/

	// ヒット位置とプレイヤー位置との距離を比較
	float hitdist = DirectX::XMVectorGetX(
		DirectX::XMVector3Length(
			DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&hitpos), DirectX::XMLoadFloat3(&RayStart))));

	float playerdist = DirectX::XMVectorGetX(
		DirectX::XMVector3Length(
			DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&playerRef.lock()->GetPosition()), DirectX::XMLoadFloat3(&this->GetPosition()))));

	if (isHit && state != State::Attack)
	{
		moveSpeed = 0;
		state = State::Attack;

		batteryManager::Instance().stop();
		Audio3DSystem::Instance().StopByTag("enemy_run");
		Audio3DSystem::Instance().StopByTag("enemy_walk");
		Animationplay();
		return;
	}

	DirectX::XMVECTOR Forward;
	for (int i = 0; i < model->GetNodes().size(); i++)
	{
		if (std::strcmp(model->GetNodes().at(i).name, "head_end_FK") == 0)
		{
			DirectX::XMFLOAT4X4 transform = model->GetNodes().at(i).globalTransform;
			DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&transform);
			Forward = M.r[2];
		}
	}

	DirectX::XMVECTOR v1v = DirectX::XMVector3Normalize(Forward);
	DirectX::XMVECTOR v2v = DirectX::XMVector3Normalize((DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&playerRef.lock()->GetPosition()), DirectX::XMLoadFloat3(&this->GetPosition()))));



	if (playerdist < searchRange && !isTrackingPlayer && !isPlayerInView)
	{
		// 経路をリセットし、新たに探索開始
		stage->path.clear();
		route.clear();
		currentTargetIndex = 0;

		Goal::Instance().SetPosition(playerRef.lock()->GetPosition());
		Start::Instance().SetPosition(this->position);
		SearchAI::Instance().trackingSearch(stage);

		int current = stage->NearWayPointIndex(Goal::Instance().GetPosition());
		int start = stage->NearWayPointIndex(this->position);

		refinePath(start, current); // 経路を作成

		state = State::feeling;
		Animationplay();
		isTrackingPlayer = true;
	}

	float angle = acosf(XMVectorGetX(XMVector3Dot(v1v, v2v))) / 0.01745f;

	char buf[256];
	/*sprintf_s(buf, sizeof(buf), "angle%f\n", angle);
	OutputDebugStringA(buf);*/

	// プレイヤーが見えているか近づいているなら
	if ((loocking && playerdist < lockonRange) && angle <= 45.0f)
	{
		isReverseTraced = false;

		if (loocking && playerdist < lockonRange && !isPlayerInView)
		{
			// 経路をリセットし、新たに探索開始
			stage->path.clear();
			route.clear();
			currentTargetIndex = 0;

			Goal::Instance().SetPosition(playerRef.lock()->GetPosition());
			Start::Instance().SetPosition(this->position);
			SearchAI::Instance().trackingSearch(stage);

			int current = stage->NearWayPointIndex(Goal::Instance().GetPosition());
			int start = stage->NearWayPointIndex(this->position);

			refinePath(start, current); // 経路を作成

			state = State::detection;
			Animationplay();
			isPlayerInView = true;

			if (!route.empty())
			{
				targetPosition = route[0];
				JageDirection(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&targetPosition), DirectX::XMLoadFloat3(&this->position)));
			}
		}
		else
		{
			// 追跡中はリアルタイムに再探索
			stage->path.clear();
			Goal::Instance().SetPosition(playerRef.lock()->GetPosition());
			if (!route.empty())
				Start::Instance().SetPosition(route.back());
			SearchAI::Instance().trackingSearch(stage);

			int current = stage->NearWayPointIndex(Goal::Instance().GetPosition());
			int start = stage->NearWayPointIndex(Start::Instance().GetPosition());

			refinePath(start, current);


			/*char checks[256];
			sprintf_s(checks, sizeof(checks), "add\n");
			OutputDebugStringA(checks);*/
		}

	}

	int current, start;

	// 敵の状態に応じて処理を分岐
	switch (state)
	{
	case State::Roaming:

		Updatemovement(elapsedTime); // 経路に沿って移動
		break;

	case State::turn:

		Audio3DSystem::Instance().StopByTag("enemy_run");
		Audio3DSystem::Instance().StopByTag("enemy_walk");
		if (animationcontroller.GetEndAnimation())
		{
			state = State::Roaming;
			if (isTrackingPlayer|| isPlayerInView)
			{
				Audio3DSystem::Instance().PlayByTag("enemy_run");
			}
			else
			{
				Audio3DSystem::Instance().PlayByTag("enemy_walk");
			}
			Animationplay();
		}
		break;

	case State::Idle:
	{
		Audio3DSystem::Instance().StopByTag("enemy_run");
		Audio3DSystem::Instance().StopByTag("enemy_walk");

		// ランダムな目標地点を設定し経路探索
		Goal::Instance().SetPosition(stage->GetIndexWayPoint(stage->RandomPoint()));

#if 0
		//（デバッグ：Tキー）
		if (GetAsyncKeyState('T') & 0x8000)
		{
			Start::Instance().SetPosition(this->position);
			SearchAI::Instance().freeSearch(stage);

			int current = stage->NearWayPointIndex(Goal::Instance().GetPosition());
			int start = stage->NearWayPointIndex(this->GetPosition());

			if (SearchAI::Instance().findRoot[current] == -1 || current == start)
				break;

			refinePath(start, current);
			if (!route.empty())
			{
				targetPosition = route[0];
			}
			state = State::Roaming;
			Animationplay();
		}
#else
		Start::Instance().SetPosition(this->position);
		SearchAI::Instance().freeSearch(stage);

		current = stage->NearWayPointIndex(Goal::Instance().GetPosition());
		start = stage->NearWayPointIndex(this->GetPosition());

		if (SearchAI::Instance().findRoot[current] == -1 || current == start)
			break;

		refinePath(start, current);
		if (!route.empty())
		{
			targetPosition = route[0];
		}
		state = State::Roaming;
		Animationplay();
#endif

		break;
	}
	case State::detection:
		Audio3DSystem::Instance().StopByTag("enemy_walk");
		if (animationcontroller.GetEndAnimation())
		{
			moveSpeed = TRACKING_SPEED; // 追跡スピード
			state = State::Roaming;
			Audio3DSystem::Instance().PlayByTag("enemy_run");
			Animationplay();
		}
		break;

	case State::feeling:
		Audio3DSystem::Instance().StopByTag("enemy_walk");
		Audio3DSystem::Instance().PlayByTag("enemy_run");
		moveSpeed = FEELING_SPEED; // 近距離反応スピード
		state = State::Roaming;
		Animationplay();

		break;

	case State::miss:

		isTrackingPlayer = false;
		isPlayerInView = false;
		if (animationcontroller.GetEndAnimation())
		{
			moveSpeed = USUAL_SPEED; // 通常速度に戻す
			state = State::Idle;
			Animationplay();
		}

		if (((loocking && playerdist < lockonRange) || (loocking && playerdist < searchRange)))
		{
			state = State::detection;
			JageDirection(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&targetPosition), DirectX::XMLoadFloat3(&position)));
			Animationplay();
		}
		break;

	case State::Attack:
		if (animationcontroller.GetEndAnimation())
		{
			//SceneManager::instance().ChangeScene(new SceneLoading(new Game_Over));
			isDead = true;

			batteryManager::Instance().ClearBattery();
			moveSpeed = 0;
		}
		break;
	}

	// 行列更新とモデルの描画準備
	UpdateTransform();
	model->UpdateTransform();
	animationcontroller.UpdateAnimation(elapsedTime);
}


// 経路に沿って敵を移動させる処理
void Enemy::Updatemovement(float elapsedTime)
{
	if (route.empty() || currentTargetIndex >= route.size())
	{
		if (isTrackingPlayer || isPlayerInView || isReverseTraced)
		{
			// 追跡終了時の処理
			state = State::miss;
			Animationplay();
			currentTargetIndex = 0;

			if (!route.empty())
			{
				route.clear();
			}
			if (!stage->path.empty())
			{

				stage->path.clear();
			}
			return;
		}
		else
		{
			// 経路を使い果たしたときの待機処理
			state = State::Idle;
			currentTargetIndex = 0;

			if (!route.empty() || !stage->path.empty())
			{
				stage->path.clear();
				route.clear();
			}
			isReverseTraced = false;
			return;
		}
	}

	bool nearTarget = false;
	// ターゲット方向に向けた移動
	DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&targetPosition);
	if (DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&playerRef.lock().get()->GetPosition()), posVec))) < 3.0f && loocking)
	{
		targetVec = DirectX::XMLoadFloat3(&playerRef.lock().get()->GetPosition());
		JageDirection(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&targetPosition), posVec));
		Animationplay();
		nearTarget = true;
	}
	else
	{
		nearTarget = false;
	}
	DirectX::XMVECTOR dir = DirectX::XMVectorSubtract(targetVec, posVec);
	DirectX::XMFLOAT3 dirf;
	DirectX::XMStoreFloat3(&dirf, dir);
	if (!nearTarget && std::abs(dirf.x) > 0.2f && std::abs(dirf.z) > 0.2f)
	{
		if (std::abs(dirf.x) > std::abs(dirf.z)) {
			dirf.z = 0;
		}
		else {
			dirf.x = 0;
		}
	}
	dir = DirectX::XMLoadFloat3(&dirf);
	DirectX::XMVECTOR dirNorm = DirectX::XMVector3Normalize(dir);
	DirectX::XMVECTOR moveVec = DirectX::XMVectorScale(dirNorm, moveSpeed * elapsedTime);
	posVec = DirectX::XMVectorAdd(posVec, moveVec);
	DirectX::XMStoreFloat3(&position, posVec);

	if (nearTarget)
	{
		return;
	}

	// ターゲット地点に近づいたら次の目的地へ
	float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(dir));
	if (distance < 0.1f)
	{
		int from = stage->NearWayPointIndex(route[currentTargetIndex]);
		currentTargetIndex++;
		if (currentTargetIndex < route.size())
		{
			int to = stage->NearWayPointIndex(route[currentTargetIndex]);
			stage->AddEdgecost(from, to);
		}
		if (currentTargetIndex < route.size())
		{
			targetPosition = route[currentTargetIndex];
			float measurement = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&targetPosition), posVec)));
			while (measurement < 0.1f && currentTargetIndex <= route.size())
			{
				if (measurement < 0.1f) currentTargetIndex++;
			}
		}
		if (currentTargetIndex >= route.size())
		{
			return;
		}
		targetPosition = route[currentTargetIndex];
		JageDirection(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&targetPosition), posVec));
		state = State::turn;
		Animationplay();
	}
}

// Dijkstra探索後の経路復元処理
void Enemy::refinePath(int start, int current)
{
	// スタートからゴールまでのパスを逆順にたどる
	while (current != start)
	{
		stage->path.push_back(current);
		current = SearchAI::Instance().findRoot[current];
	}
	stage->path.push_back(start);

	// パスを正しい順に並べ替え
	std::reverse(stage->path.begin(), stage->path.end());

	// 経路を waypoint 座標に変換して route に格納
	for (auto i : stage->path)
	{
		if (!route.empty())
		{
			if (DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&stage->wayPoint[i]->position), DirectX::XMLoadFloat3(&route.back())))) < 0.3f)
			{
				continue;
			}
		}
		this->Addroute(stage->wayPoint[i]->position);
	}
}

void Enemy::remote_sensing(DirectX::XMFLOAT3 pos)
{
	if (isReverseTraced)
	{
		// 経路をリセットし、新たに探索開始
		stage->path.clear();
		route.clear();
		currentTargetIndex = 0;

		Goal::Instance().SetPosition(pos);
		Start::Instance().SetPosition(this->position);
		SearchAI::Instance().trackingSearch(stage);

		int current = stage->NearWayPointIndex(Goal::Instance().GetPosition());
		int start = stage->NearWayPointIndex(this->position);

		refinePath(start, current); // 経路を作成

		state = State::Roaming;
		isReverseTraced = true;
	}
	else
	{
		stage->path.clear();
		Goal::Instance().SetPosition(pos);
		if (!route.empty())
			Start::Instance().SetPosition(route.back());
		SearchAI::Instance().trackingSearch(stage);

		int current = stage->NearWayPointIndex(Goal::Instance().GetPosition());
		int start = stage->NearWayPointIndex(Start::Instance().GetPosition());

		refinePath(start, current);

		state = State::Roaming;
	}
}

void Enemy::JageDirection(DirectX::XMVECTOR dir)
{
	olddirection = direction;
	DirectX::XMFLOAT3 dirf;
	DirectX::XMStoreFloat3(&dirf, dir);

	if (std::abs(dirf.x) > std::abs(dirf.z)) {
		dirf.z = 0;
	}
	else {
		dirf.x = 0;
	}

	if (std::abs(dirf.x) > std::abs(dirf.z)) {
		direction = (dirf.x > 0) ? Direction::E : Direction::W;
	}
	else {
		direction = (dirf.z > 0) ? Direction::N : Direction::S;
	}
	return;
}

void Enemy::Animationplay()
{
	if (state == State::Attack)
	{
		// ベクトル変換
		DirectX::XMVECTOR enemy = DirectX::XMLoadFloat3(&position);
		DirectX::XMVECTOR player = DirectX::XMLoadFloat3(&playerRef.lock()->GetPosition());

		// 向きベクトル（プレイヤー → 敵）
		DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(player, enemy));

		// XZ 平面上で角度を求める
		float dx = DirectX::XMVectorGetX(dir);
		float dz = DirectX::XMVectorGetZ(dir);

		// atan2 で Y軸回転角を計算（Zが前、Xが右の座標系）
		angle.y = std::atan2(dx, dz);
		animationcontroller.PlayAnimation("attack", false);
		animationcontroller.SetAnimationSecondScale(1.2f);
		angle.y += 6 * 0.01745f;

		int deathState = playerRef.lock()->GetDeathState();
		if(deathState == 0)
			angle.x += 3 * 0.01745f;
		else if(deathState == 1)
			angle.x += 10 * 0.01745f;
	}
	switch (direction)
	{
	case Enemy::N:

		switch (state)
		{
		case Enemy::State::Idle:
			break;
		case Enemy::State::Roaming:
			if (isTrackingPlayer)
			{
				animationcontroller.PlayAnimation("run_front", true);
			}
			else
			{
				animationcontroller.PlayAnimation("walk_front", true);
			}
			break;
		case Enemy::State::detection:
			animationcontroller.PlayAnimation("findTarget_front", false);
			break;
		case Enemy::State::miss:
			animationcontroller.PlayAnimation("lostTarget_front", false);
			break;
		case Enemy::State::turn:
			if (olddirection == Direction::E)
			{
				animationcontroller.PlayAnimation("rotate_rightToFront", false);
			}
			else if (olddirection == Direction::W)
			{
				animationcontroller.PlayAnimation("rotate_leftToFront", false);
			}


			else if (olddirection == Direction::S)
			{
				animationcontroller.PlayAnimation("rotate_backToFront", false);
			}
			else
			{
				state = State::Roaming;
				Animationplay();
			}
			break;
		default:
			break;
		}
		break;

	case Enemy::S:

		switch (state)
		{
		case Enemy::State::Idle:
			break;
		case Enemy::State::Roaming:
			if (isTrackingPlayer)
			{
				animationcontroller.PlayAnimation("run_back", true);
			}
			else
			{
				animationcontroller.PlayAnimation("walk_back", true);
			}
			break;
		case Enemy::State::detection:
			animationcontroller.PlayAnimation("findTarget_back", false);
			break;
		case Enemy::State::miss:
			animationcontroller.PlayAnimation("lostTarget_back", false);
			break;
		case Enemy::State::turn:
			if (olddirection == Direction::E)
			{
				animationcontroller.PlayAnimation("rotate_rightToBack", false);
			}
			else if (olddirection == Direction::W)
			{
				animationcontroller.PlayAnimation("rotate_leftToBack", false);
			}
			else if (olddirection == Direction::N)
			{
				animationcontroller.PlayAnimation("rotate_frontToBack", false);
			}
			else
			{
				state = State::Roaming;
				Animationplay();
			}
			break;
		default:
			break;
		}
		break;

	case Enemy::W:

		switch (state)
		{
		case Enemy::State::Idle:
			break;
		case Enemy::State::Roaming:
			if (isTrackingPlayer)
			{
				animationcontroller.PlayAnimation("run_left", true);
			}
			else
			{
				animationcontroller.PlayAnimation("walk_left", true);
			}
			break;
		case Enemy::State::detection:
			animationcontroller.PlayAnimation("findTarget_left", false);
			break;
		case Enemy::State::miss:
			animationcontroller.PlayAnimation("lostTarget_left", false);
			break;
		case Enemy::State::turn:
			if (olddirection == Direction::N)
			{
				animationcontroller.PlayAnimation("rotate_frontToLeft", false);
			}
			else if (olddirection == Direction::S)
			{
				animationcontroller.PlayAnimation("rotate_backToLeft", false);
			}
			else if (olddirection == Direction::E)
			{
				animationcontroller.PlayAnimation("rotate_rightToLeft", false);
			}
			else
			{
				state = State::Roaming;
				Animationplay();
			}
			break;
		default:
			break;
		}
		break;

	case Enemy::E:

		switch (state)
		{
		case Enemy::State::Idle:
			break;
		case Enemy::State::Roaming:
			if (isTrackingPlayer)
			{
				animationcontroller.PlayAnimation("run_right", true);
			}
			else
			{
				animationcontroller.PlayAnimation("walk_right", true);
			}
			break;
		case Enemy::State::detection:
			animationcontroller.PlayAnimation("findTarget_right", false);
			break;
		case Enemy::State::miss:
			animationcontroller.PlayAnimation("lostTarget_right", false);
			break;
		case Enemy::State::turn:
			if (olddirection == Direction::S)
			{
				animationcontroller.PlayAnimation("rotate_backToRight", false);
			}
			else if (olddirection == Direction::N)
			{
				animationcontroller.PlayAnimation("rotate_frontToRight", false);
			}
			else if (olddirection == Direction::W)
			{
				animationcontroller.PlayAnimation("rotate_leftToRight", false);
			}
			else
			{
				state = State::Roaming;
				Animationplay();
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void Enemy::SetDifficulty()
{
	switch (Difficulty::Instance().GetDifficulty())
	{
	case Difficulty::tutorial:
		searchRange = EASY_SR;
		lockonRange = EASY_LR;
		break;
	case Difficulty::normal:
	case Difficulty::hard:
		searchRange = NORMAL_AND_HARD_SR;
		lockonRange = NORMAL_AND_HARD_LR;
		break;
	default:
		break;
	}
}

void Enemy::play_Enemy_Sound()
{
	switch (state)
	{
	case Enemy::State::Roaming:
		if (isReverseTraced)
		{
			Audio3DSystem::Instance().PlayByTag("enemy_run");
		}
		else
		{
			Audio3DSystem::Instance().PlayByTag("enemy_walk");
		}
		break;
	case Enemy::State::Attack:
		break;
	default:
		break;
	}
}

void Enemy::detectPlayerPosition()
{
	// 経路をリセットし、新たに探索開始
	stage->path.clear();
	route.clear();
	currentTargetIndex = 0;

	Goal::Instance().SetPosition(playerRef.lock()->GetPosition());
	Start::Instance().SetPosition(this->position);
	SearchAI::Instance().trackingSearch(stage);

	int current = stage->NearWayPointIndex(Goal::Instance().GetPosition());
	int start = stage->NearWayPointIndex(this->position);

	refinePath(start, current); // 経路を作成

	state = State::feeling;
	Animationplay();
	isTrackingPlayer = true;
}

// デバッグ描画
void Enemy::DrawDebug()
{
	ImGui::Begin("Enemy Info");

	// positionを表示
	ImGui::Text("Position: X=%f,Y=%f", this->position.x,this->position.y);
	//ImGui::DragFloat2("cameraShakeScale", &cameraShakeScale.x);
	ImGui::Text("state: %c", this->state);
	ImGui::Text("currentTargetIndex: %d", this->currentTargetIndex);
	/*if (route.size() > currentTargetIndex)
	{
		ImGui::Text("current_ruto_pos: %f,%f", this->route[currentTargetIndex].x, this->route[currentTargetIndex].y);
	}*/
	ImGui::End();
}

// モデル描画処理
void Enemy::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	textures->Set(rc);
	renderer->Render(rc, world, model.get(), ShaderId::Custom);
	textures->Clear(rc);
}
