#include"Stage.h"

//�R���X�g���N�^
Stage::Stage()
{
    //�X�e�[�W���f����ǂݍ���
#if 1
    model = new Model("Data/Model/Stage/stage.mdl");
#else
    model = new Model("Data/Model/Stage/ExampleStage.mdl");
#endif

    //scale = { 1.5f,1.5f,1.5f };
    //scale = { 2,2,2 };
    scale = { 1,1,1 };

    //�X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.x);
    //��]�s��
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    //�ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    //�R�̍s���g�ݍ��킹�A���[���h�s����쐬
    DirectX::XMMATRIX W = S * R * T;
    //�v�Z�������[���h�s������o��
    DirectX::XMStoreFloat4x4(&world, W);

    DestinationPointSet();
}
Stage::~Stage()
{
    //�X�e�[�W���f����j��
    delete model;
}
//�X�V����
void Stage::Update(float elapsedTime)
{

}
//�`��
void Stage::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    //�����_���Ƀ��f����`�悵�Ă��炤
    renderer->Render(rc, world, model, ShaderId::Lambert);
}



////////////////////////////////////////////////////////////////////////////////////////
//�o�H�T���p

// �E�F�C�|�C���g�ƃG�b�W�̐ݒ�
void Stage::DestinationPointSet()
{
    //WayPoint����
    wayPoint[0] = std::make_shared<WayPoint>(0, DirectX::XMFLOAT3{ 0,0,0 });
    wayPoint[1] = std::make_shared<WayPoint>(1, DirectX::XMFLOAT3{ 15,0,9 });
    wayPoint[2] = std::make_shared<WayPoint>(2, DirectX::XMFLOAT3{ 10,0,2 });
    wayPoint[3] = std::make_shared<WayPoint>(3, DirectX::XMFLOAT3{ 4,0,12 });
    wayPoint[4] = std::make_shared<WayPoint>(4, DirectX::XMFLOAT3{ 22,0,22 });


    //
    wayPoint[0]->AddEdge(wayPoint[1].get());
    wayPoint[0]->AddEdge(wayPoint[3].get());
    wayPoint[1]->AddEdge(wayPoint[2].get());
    wayPoint[2]->AddEdge(wayPoint[4].get());
    wayPoint[3]->AddEdge(wayPoint[4].get());
}

// �C���f�b�N�X�ԍ�����E�F�C�|�C���g�̍��W���擾
DirectX::XMFLOAT3 Stage::GetIndexWayPoint(int index)
{

    return wayPoint[index]->position;
}

// ���W�����ԋ߂��E�F�C�|�C���g�̃C���f�b�N�X���擾
int Stage::NearWayPointIndex(DirectX::XMFLOAT3 target)
{
    float minLength = FLT_MAX;
    float length = 0.0f;
    int index = -1;
    // VECTOR�ɕϊ�
    DirectX::XMVECTOR targetPos = DirectX::XMLoadFloat3(&target);

    for (int i = 0; i < MAX_WAY_POINT; ++i)
    {
        DirectX::XMVECTOR point = DirectX::XMLoadFloat3(&(wayPoint[i]->position));
        // ���������߂�
        DirectX::XMVECTOR vector = DirectX::XMVectorSubtract(targetPos, point);
        DirectX::XMVECTOR vectorLength = DirectX::XMVector3Length(vector);
        DirectX::XMStoreFloat(&length, vectorLength);

        // ���߂��������ۑ����Ă�����̂�菬�������
        if (minLength > length)
        {
            // �l���X�V
            minLength = length;
            index = i;
        }
    }
    return index;
}