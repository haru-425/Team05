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