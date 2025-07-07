#include "Enemy.h"
#include "Pursuer/Object.h"
#include "Pursuer/SearchAI.h"
#include "Player/player.h"
#include "./Collision.h"

// コンストラクタ：プレイヤーとステージへの参照を保持し、モデルを読み込む
Enemy::Enemy(std::shared_ptr<Player> playerRef, Stage* stage)
{
    this->stage = stage;
    this->playerRef = playerRef;
    model = new Model("Data/Model/Slime/Slime.mdl"); // スライムモデルを読み込み

    scale.x = scale.y = scale.z = 0.01f; // スケール設定（非常に小さい）
    radius = 0.5f;                        // 衝突用の半径

    position = { 0.0f, 0.0f, 0.0f };      // 初期位置
    viewPoint = 1.5f;                     // 目線の高さ
}

Enemy::~Enemy()
{
    // モデルのメモリ解放が必要な場合はここで行う
}

// 毎フレーム呼び出される更新処理
void Enemy::Update(float elapsedTime)
{
    // 敵の向きをプレイヤーに合わせるために、ビュー行列からZ軸方向を取得
    DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&world);
    DirectX::XMVECTOR Forward = M.r[2];

    float x = DirectX::XMVectorGetX(Forward);
    float y = DirectX::XMVectorGetY(Forward);
    float z = DirectX::XMVectorGetZ(Forward);

    pitch = asinf(y);             // 上下の向き
    yaw = atan2f(x, z);           // 左右の向き

    // プレイヤーとのレイキャスト処理
    const DirectX::XMFLOAT3 RayStart = this->GetPosition();
    const DirectX::XMFLOAT3 RayGoal = playerRef.lock()->GetPosition();

    DirectX::XMFLOAT3 hitpos, n;
    loocking = Collision::RayCast(RayStart, RayGoal, stage->GetWorld(), stage->GetModel(), hitpos, n);

    // ヒット位置とプレイヤー位置との距離を比較
    float hitdist = DirectX::XMVectorGetX(
        DirectX::XMVector3Length(
            DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&hitpos), DirectX::XMLoadFloat3(&RayStart))));

    float playerdist = DirectX::XMVectorGetX(
        DirectX::XMVector3Length(
            DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&playerRef.lock()->GetPosition()), DirectX::XMLoadFloat3(&this->GetPosition()))));

    // プレイヤーが見えているか近づいているなら
    if ((loocking && hitdist < lockonRange) || playerdist < searchRange)
    {
        if (!isTrackingPlayer)
        {
            // 経路をリセットし、新たに探索開始
            stage->path.clear();
            route.clear();

            Goal::Instance().SetPosition(playerRef.lock()->GetPosition());
            Start::Instance().SetPosition(this->position);
            SearchAI::Instance().DijkstraSearch(stage);

            int current = stage->NearWayPointIndex(Goal::Instance().GetPosition());
            int start = stage->NearWayPointIndex(this->position);

            refinePath(start, current); // 経路を作成

            // ステート遷移
            if (loocking && hitdist < lockonRange)
                state = State::detection;
            else if (playerdist < searchRange)
                state = State::feeling;

            isTrackingPlayer = true;
        }
        else
        {
            // 追跡中はリアルタイムに再探索
            stage->path.clear();
            Goal::Instance().SetPosition(playerRef.lock()->GetPosition());
            if (!route.empty())
                Start::Instance().SetPosition(route.back());
            SearchAI::Instance().DijkstraSearch(stage);

            int current = stage->NearWayPointIndex(Goal::Instance().GetPosition());
            int start = stage->NearWayPointIndex(Start::Instance().GetPosition());

            refinePath(start, current);
        }
    }

    // 敵の状態に応じて処理を分岐
    switch (state)
    {
    case State::Roaming:
        Updatemovement(elapsedTime); // 経路に沿って移動
        break;

    case State::turn:
        if (true)
        {
            state = State::Roaming;
        }
        break;

    case State::Idle:
        // ランダムな目標地点を設定し経路探索（デバッグ：Tキー）
        Goal::Instance().SetPosition(stage->GetIndexWayPoint(rand() % (MAX_WAY_POINT - 1) + 1));

        if (GetAsyncKeyState('T') & 0x8000)
        {
            Start::Instance().SetPosition(this->position);
            SearchAI::Instance().DijkstraSearch(stage);

            int current = stage->NearWayPointIndex(Goal::Instance().GetPosition());
            int start = stage->NearWayPointIndex(this->GetPosition());

            if (SearchAI::Instance().findRoot[current] == -1 || current == start)
                break;

            refinePath(start, current);
            state = State::Roaming;
        }
        break;

    case State::detection:
        if (true)
        {
            moveSpeed = TRACKING_SPEED; // 追跡スピード
            state = State::Roaming;
        }
        break;

    case State::feeling:
        if (true)
        {
            moveSpeed = FEELING_SPEED; // 近距離反応スピード
            state = State::Roaming;
        }
        break;

    case State::miss:

        if (true)
        {
            isTrackingPlayer = false;
            moveSpeed = USUAL_SPEED; // 通常速度に戻す
            state = State::Idle;
        }
        break;
    }

    // 行列更新とモデルの描画準備
    UpdateTransform();
    model->UpdateTransform();
}


// 経路に沿って敵を移動させる処理
void Enemy::Updatemovement(float elapsedTime)
{
    if (route.empty() || currentTargetIndex >= route.size())
    {
        if (isTrackingPlayer)
        {
            // 追跡終了時の処理
            state = State::miss;
            currentTargetIndex = 0;

            if (!route.empty() || !stage->path.empty())
            {
                stage->path.clear();
                route.clear();
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
            return;
        }
    }

    // ターゲット方向に向けた移動
    DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&targetPosition);
    DirectX::XMVECTOR dir = DirectX::XMVectorSubtract(targetVec, posVec);

    DirectX::XMVECTOR dirNorm = DirectX::XMVector3Normalize(dir);
    DirectX::XMVECTOR moveVec = DirectX::XMVectorScale(dirNorm, moveSpeed * elapsedTime);
    posVec = DirectX::XMVectorAdd(posVec, moveVec);
    DirectX::XMStoreFloat3(&position, posVec);

    // ターゲット地点に近づいたら次の目的地へ
    float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(dir));
    if (distance < 0.1f)
    {
        currentTargetIndex++;
        if (currentTargetIndex < route.size())
        {
            targetPosition = route[currentTargetIndex];
            float measurement = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&targetPosition), posVec)));
            while (measurement<0.5f && currentTargetIndex <= route.size())
            {
                currentTargetIndex++;
            }
        }
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
        this->Addroute(stage->wayPoint[i]->position);
    }
}

// デバッグ描画（未実装）
void Enemy::DrawDebug()
{
}

// モデル描画処理
void Enemy::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    renderer->Render(rc, world, model, ShaderId::Lambert);
}
