#include "Enemy.h"
#include <cmath>
#include <iostream>
#include <random>
#include "Pursuer/Object.h"
#include "Pursuer/SearchAI.h"
#include "Player/player.h"
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
    radius = 0.5f;                        // 衝突用の半径

    viewPoint = 1.5f;                     // 目線の高さ

    position = stage->GetIndexWayPoint(33);      // 初期位置
    Goal::Instance().SetPosition(stage->GetIndexWayPoint(61));

    Start::Instance().SetPosition(this->position);
    SearchAI::Instance().DijkstraSearch(stage);

    int current = stage->NearWayPointIndex(Goal::Instance().GetPosition());
    int start = stage->NearWayPointIndex(this->position);


    refinePath(start, current); // 経路を作成

    // ステート遷移
    targetPosition = route[0];
    state = State::Roaming;
    Animationplay();

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
    const DirectX::XMFLOAT3 RayStart = { this->GetPosition().x, this->GetPosition().y + viewPoint, this->GetPosition().z };
    const DirectX::XMFLOAT3 RayGoal = { playerRef.lock()->GetPosition().x, playerRef.lock()->GetPosition().y + viewPoint, playerRef.lock()->GetPosition().z };

    DirectX::XMFLOAT3 hitpos, n;
    //bool a = Collision::RayCast(RayStart, RayGoal, stage->GetWorld(), stage->GetModel(), hitpos, n);      //(デバッグ用)
    loocking = !(Collision::RayCast(RayStart, RayGoal, stage->GetWorld(), stage->GetModel(), hitpos, n));

    // ヒット位置とプレイヤー位置との距離を比較
    float hitdist = DirectX::XMVectorGetX(
        DirectX::XMVector3Length(
            DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&hitpos), DirectX::XMLoadFloat3(&RayStart))));

    float playerdist = DirectX::XMVectorGetX(
        DirectX::XMVector3Length(
            DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&playerRef.lock()->GetPosition()), DirectX::XMLoadFloat3(&this->GetPosition()))));

    // プレイヤーが見えているか近づいているなら
    if (((loocking && playerdist < lockonRange) || playerdist < searchRange ) && state!=State::miss)
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
            if (loocking && playerdist < lockonRange)
            {
                state = State::detection;
                Animationplay();
            }
            else if (playerdist < searchRange)
            {
                state = State::feeling;
                Animationplay();
            }

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

    int current, start;
    // ハードウェア由来のランダムシードを取得
    std::random_device rd;

    // メルセンヌツイスタ（高性能な乱数生成器）にシードを与える
    std::mt19937 gen(rd());

    // 敵の状態に応じて処理を分岐
    switch (state)
    {
    case State::Roaming:
        Updatemovement(elapsedTime); // 経路に沿って移動
        break;

    case State::turn:
        if (animationcontroller.GetEndAnimation())
        {
            state = State::Roaming;
            Animationplay();
        }
        break;

    case State::Idle:
    {
        std::uniform_int_distribution<> dist(0, MAX_WAY_POINT - 1);
        int value = dist(gen);

        // ランダムな目標地点を設定し経路探索
        Goal::Instance().SetPosition(stage->GetIndexWayPoint(value + 1));

#if 0
        //（デバッグ：Tキー）
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
            Animationplay();
        }
#endif

#if 1
        Start::Instance().SetPosition(this->position);
        SearchAI::Instance().DijkstraSearch(stage);

        current = stage->NearWayPointIndex(Goal::Instance().GetPosition());
        start = stage->NearWayPointIndex(this->GetPosition());

        if (SearchAI::Instance().findRoot[current] == -1 || current == start)
            break;

        refinePath(start, current);
        state = State::Roaming;
        Animationplay();
#endif

        break;
    }
    case State::detection:
        if (animationcontroller.GetEndAnimation())
        {
            moveSpeed = TRACKING_SPEED; // 追跡スピード
            state = State::Roaming;
            Animationplay();
        }
        break;

    case State::feeling:

        moveSpeed = FEELING_SPEED; // 近距離反応スピード
        state = State::Roaming;
        Animationplay();

        break;

    case State::miss:

        if (animationcontroller.GetEndAnimation())
        {
            isTrackingPlayer = false;
            moveSpeed = USUAL_SPEED; // 通常速度に戻す
            state = State::Idle;
            Animationplay();
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
        if (isTrackingPlayer)
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
            return;
        }
    }

    bool nearTarget = false;
    // ターゲット方向に向けた移動
    DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&targetPosition);
    if (DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&playerRef.lock().get()->GetPosition()), posVec))) < 3.0f)
    {
        targetVec = DirectX::XMLoadFloat3(&playerRef.lock().get()->GetPosition());
        JageDirection(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&targetPosition), posVec));
        Animationplay();
        nearTarget = true;
    }
    else
    {
        nearTarget=false;
    }
    DirectX::XMVECTOR dir = DirectX::XMVectorSubtract(targetVec, posVec);

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
        currentTargetIndex++;
        if (currentTargetIndex < route.size())
        {
            targetPosition = route[currentTargetIndex];
            float measurement = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&targetPosition), posVec)));
            while (measurement < 0.1f && currentTargetIndex <= route.size())
            {
                currentTargetIndex++;
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
       /* if (stage->NearWayPointIndex(targetPosition) == 12 || stage->NearWayPointIndex(targetPosition) == 21)
        {
            int x = 19;
        }*/
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

void Enemy::JageDirection(DirectX::XMVECTOR dir)
{
    olddirection = direction;
    DirectX::XMFLOAT3 dirf;
    DirectX::XMStoreFloat3(&dirf, dir);
    if (dirf.x > 0.1f && dirf.z < dirf.x)
    {
        direction = Direction::E;
    }
    else if (dirf.x < -0.1f && dirf.z > dirf.x)
    {
        direction = Direction::W;
    }
    else if (dirf.z > 0.1f && dirf.z > dirf.x)
    {
        direction = Direction::N;
    }
    else if (dirf.z < -0.1f && dirf.z < dirf.x)
    {
        direction = Direction::S;
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
        DirectX::XMVECTOR dir = DirectX::XMVectorSubtract(player, enemy);

        // XZ 平面上で角度を求める
        float dx = DirectX::XMVectorGetX(dir);
        float dz = DirectX::XMVectorGetZ(dir);

        // atan2 で Y軸回転角を計算（Zが前、Xが右の座標系）
        angle.y = std::atan2(dx, dz);
        animationcontroller.PlayAnimation("attack", false);
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
            if (olddirection==Direction::E)
            {
                animationcontroller.PlayAnimation("rotate_rightToFront", false);
            }
            else if (olddirection==Direction::W)
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
    case Difficulty::easy:
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

// デバッグ描画（未実装）
void Enemy::DrawDebug()
{
    ImGui::Begin("Enemy Info");

    // positionを表示
    ImGui::Text("Position: X=%d",this->state);
    ImGui::End();
}

// モデル描画処理
void Enemy::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    textures->Set(rc);
    renderer->Render(rc, world, model.get(), ShaderId::Custom);
    textures->Clear(rc);
}
