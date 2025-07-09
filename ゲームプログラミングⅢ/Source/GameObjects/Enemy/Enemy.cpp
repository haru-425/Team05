#include "Enemy.h"
#include "Pursuer/Object.h"
#include "Pursuer/SearchAI.h"
#include "Player/player.h"
#include "./Collision.h"
#include "imgui.h"                    // ImGui�̊�{�@�\
#include "imgui_impl_win32.h"        // Win32�p�o�b�N�G���h
#include "imgui_impl_dx11.h"         // DirectX11�p�o�b�N�G���h

// �R���X�g���N�^�F�v���C���[�ƃX�e�[�W�ւ̎Q�Ƃ�ێ����A���f����ǂݍ���
Enemy::Enemy(std::shared_ptr<Player> playerRef, Stage* stage)
{
    this->stage = stage;
    this->playerRef = playerRef;
    //model = std::make_shared<Model>("Data/Model/Slime/Slime.mdl");
    model = std::make_shared<Model>("Data/Model/test/enemy_motion.mdl");
    this->animationcontroller.SetModel(model);
    this->animationcontroller.SetAnimationPlaying(true);
    scale.x = scale.y = scale.z = 0.01f; // �X�P�[���ݒ�i���ɏ������j
    radius = 0.5f;                        // �Փ˗p�̔��a

    position = { 0.0f, 0.0f, 0.0f };      // �����ʒu
    viewPoint = 1.5f;                     // �ڐ��̍���
}

Enemy::~Enemy()
{
    // ���f���̃�����������K�v�ȏꍇ�͂����ōs��
}

// ���t���[���Ăяo�����X�V����
void Enemy::Update(float elapsedTime)
{
    // �G�̌������v���C���[�ɍ��킹�邽�߂ɁA�r���[�s�񂩂�Z���������擾
    DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&world);
    DirectX::XMVECTOR Forward = M.r[2];

    float x = DirectX::XMVectorGetX(Forward);
    float y = DirectX::XMVectorGetY(Forward);
    float z = DirectX::XMVectorGetZ(Forward);

    pitch = asinf(y);             // �㉺�̌���
    yaw = atan2f(x, z);           // ���E�̌���

    // �v���C���[�Ƃ̃��C�L���X�g����
    const DirectX::XMFLOAT3 RayStart = { this->GetPosition().x, this->GetPosition().y + viewPoint, this->GetPosition().z };
    const DirectX::XMFLOAT3 RayGoal = { playerRef.lock()->GetPosition().x, playerRef.lock()->GetPosition().y + viewPoint, playerRef.lock()->GetPosition().z };

    DirectX::XMFLOAT3 hitpos, n;
    //bool a = Collision::RayCast(RayStart, RayGoal, stage->GetWorld(), stage->GetModel(), hitpos, n);      //(�f�o�b�O�p)
    loocking = !(Collision::RayCast(RayStart, RayGoal, stage->GetWorld(), stage->GetModel(), hitpos, n));

    // �q�b�g�ʒu�ƃv���C���[�ʒu�Ƃ̋������r
    float hitdist = DirectX::XMVectorGetX(
        DirectX::XMVector3Length(
            DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&hitpos), DirectX::XMLoadFloat3(&RayStart))));

    float playerdist = DirectX::XMVectorGetX(
        DirectX::XMVector3Length(
            DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&playerRef.lock()->GetPosition()), DirectX::XMLoadFloat3(&this->GetPosition()))));

    // �v���C���[�������Ă��邩�߂Â��Ă���Ȃ�
    if ((loocking && playerdist < lockonRange) || playerdist < searchRange)
    {
        if (!isTrackingPlayer)
        {
            // �o�H�����Z�b�g���A�V���ɒT���J�n
            stage->path.clear();
            route.clear();

            Goal::Instance().SetPosition(playerRef.lock()->GetPosition());
            Start::Instance().SetPosition(this->position);
            SearchAI::Instance().DijkstraSearch(stage);

            int current = stage->NearWayPointIndex(Goal::Instance().GetPosition());
            int start = stage->NearWayPointIndex(this->position);

            refinePath(start, current); // �o�H���쐬

            // �X�e�[�g�J��
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
            // �ǐՒ��̓��A���^�C���ɍĒT��
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

    // �G�̏�Ԃɉ����ď����𕪊�
    switch (state)
    {
    case State::Roaming:
        Updatemovement(elapsedTime); // �o�H�ɉ����Ĉړ�
        break;

    case State::turn:
        if (animationcontroller.GetEndAnimation())
        {
            state = State::Roaming;
            Animationplay();
        }
        break;

    case State::Idle:
        // �����_���ȖڕW�n�_��ݒ肵�o�H�T��
        Goal::Instance().SetPosition(stage->GetIndexWayPoint(rand() % (MAX_WAY_POINT - 1) + 1));
#if 0
        //�i�f�o�b�O�FT�L�[�j
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

    case State::detection:
        if (animationcontroller.GetEndAnimation())
        {
            moveSpeed = TRACKING_SPEED; // �ǐՃX�s�[�h
            state = State::Roaming;
            Animationplay();
        }
        break;

    case State::feeling:

        moveSpeed = FEELING_SPEED; // �ߋ��������X�s�[�h
        state = State::Roaming;
        Animationplay();

        break;

    case State::miss:

        if (animationcontroller.GetEndAnimation())
        {
            isTrackingPlayer = false;
            moveSpeed = USUAL_SPEED; // �ʏ푬�x�ɖ߂�
            state = State::Idle;
            Animationplay();
        }
        break;
    }

    // �s��X�V�ƃ��f���̕`�揀��
    UpdateTransform();
    model->UpdateTransform();
    animationcontroller.UpdateAnimation(elapsedTime);
}


// �o�H�ɉ����ēG���ړ������鏈��
void Enemy::Updatemovement(float elapsedTime)
{
    if (route.empty() || currentTargetIndex >= route.size())
    {
        if (isTrackingPlayer)
        {
            // �ǐՏI�����̏���
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
            // �o�H���g���ʂ������Ƃ��̑ҋ@����
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
    // �^�[�Q�b�g�����Ɍ������ړ�
    DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&targetPosition);
    if (DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&playerRef.lock().get()->GetPosition()), posVec))) < 3.0f)
    {
        targetVec = DirectX::XMLoadFloat3(&playerRef.lock().get()->GetPosition());
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

    // �^�[�Q�b�g�n�_�ɋ߂Â����玟�̖ړI�n��
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
        jageDirection(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&targetPosition), posVec));
        state = State::turn;
        Animationplay();
       /* if (stage->NearWayPointIndex(targetPosition) == 12 || stage->NearWayPointIndex(targetPosition) == 21)
        {
            int x = 19;
        }*/
    }
}

// Dijkstra�T����̌o�H��������
void Enemy::refinePath(int start, int current)
{
    // �X�^�[�g����S�[���܂ł̃p�X���t���ɂ��ǂ�
    while (current != start)
    {
        stage->path.push_back(current);
        current = SearchAI::Instance().findRoot[current];
    }
    stage->path.push_back(start);

    // �p�X�𐳂������ɕ��בւ�
    std::reverse(stage->path.begin(), stage->path.end());

    // �o�H�� waypoint ���W�ɕϊ����� route �Ɋi�[
    for (auto i : stage->path)
    {
        this->Addroute(stage->wayPoint[i]->position);
    }
}

void Enemy::jageDirection(DirectX::XMVECTOR dir)
{
    olddirection = direction;
    DirectX::XMFLOAT3 dirf;
    DirectX::XMStoreFloat3(&dirf, dir);
    if (dirf.x > 0.1f)
    {
        direction = Direction::E;
    }
    else if (dirf.z > 0.1f)
    {
        direction = Direction::N;
    }
    else if (dirf.x < -0.1f)
    {
        direction = Direction::W;
    }
    else if (dirf.z < -0.1f)
    {
        direction = Direction::S;
    }
    return;
}

void Enemy::Animationplay()
{
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
            
            /*
            else if (true)
            {
                animationcontroller.PlayAnimation("", false);
            }*/
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
            /*
            else if (true)
            {
                animationcontroller.PlayAnimation("", false);
            }*/
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
            /*else if (true)
            {
                animationcontroller.PlayAnimation("", false);
            }*/
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
                animationcontroller.PlayAnimation("rotate_backToright", false);
            }
            else if (olddirection == Direction::N)
            {
                animationcontroller.PlayAnimation("rotate_frontToright", false);
            }
            /*else if (true)
            {
                animationcontroller.PlayAnimation("", false);
            }*/
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

// �f�o�b�O�`��i�������j
void Enemy::DrawDebug()
{
    ImGui::Begin("Enemy Info");

    // position��\��
    ImGui::Text("Position: X=%d",this->state);
    ImGui::End();
}

// ���f���`�揈��
void Enemy::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    renderer->Render(rc, world, model.get(), ShaderId::Lambert);
}
