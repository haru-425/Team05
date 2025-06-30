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
	// �X�e�[�g���̍X�V����
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
		//�ҋ@�X�e�[�g�֑J��
		//state = State::Idle;
		return;
	}

	// �ړ������x�N�g��
	DirectX::XMVECTOR posVec = XMLoadFloat3(&position);
	DirectX::XMVECTOR targetVec = XMLoadFloat3(&targetPosition);
	DirectX::XMVECTOR dir = DirectX::XMVectorSubtract(targetVec, posVec);

	// �ړ��ʂ��v�Z
	DirectX::XMVECTOR dirNorm = DirectX::XMVector3Normalize(dir);
	DirectX::XMVECTOR moveVec = DirectX::XMVectorScale(dirNorm, moveSpeed * elapsedTime);

	// ���݈ʒu�X�V
	posVec = DirectX::XMVectorAdd(posVec, moveVec);
	XMStoreFloat3(&position, posVec);

	// �^�[�Q�b�g�ɏ\���߂Â����玟�̃|�C���g��
	float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(dir));
	if (distance < 0.1f) { // �덷�Ŏ~�܂�Ȃ��悤��臒l
		currentTargetIndex++;
		// ���̃^�[�Q�b�g
		targetPosition = route[currentTargetIndex];
	}

	//// �ڕW�n�_�܂�XZ���ʂł̋�������
	//float vx = targetPosition.x - position.x;
	//float vz = targetPosition.z - position.z;
	//float distSq = vx * vx + vz * vz;
	//if (distSq < radius * radius)
	//{
	//	// �ҋ@�X�e�[�g�֑J��
	//	SetIdleState();
	//}

	//// �ڕW�n�_�ֈړ�
	//MoveToTarget(elapsedTime, 1.0f, 1.0f);
	//
	// //�v���C���[���G
	//if (SearchPlayer())
	//{
	//	// ����������U���X�e�[�g�֑J��
	//	SetAttackState();
	//}
}

void Enemy::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, world, model, ShaderId::Lambert);
}