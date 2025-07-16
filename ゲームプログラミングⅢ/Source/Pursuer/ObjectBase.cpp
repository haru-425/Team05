#include "ObjectBase.h"

void ObjectBase::UpdateTransform()
{
	// �X�P�[���s����쐬
	DirectX::XMMATRIX s = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// ��]�s����쐬
	DirectX::XMMATRIX r = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	// �ʒu�s����쐬
	DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	// 3�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX w = s * r * t;
	// �v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&transform, w);
}

//�C���X�^���X�擾
Start& Start::Instance() {
	static Start instance;
	return instance;
}


// �R���X�g���N�^
Start::Start() {
	position = DirectX::XMFLOAT3(0.5f, 0.0f, 0.5f);
}

// �f�X�g���N�^
Start::~Start()
{
}

// �v���C���[�X�V����
void Start::Update(float elapsedTime)
{
	// �I�u�W�F�N�g�s����X�V
	UpdateTransform();

}

// �`��
void Start::Render(RenderContext rc, ShapeRenderer* renderer)
{
	// �`��
	//PrimitiveRenderer* primitiveRenderer = Graphics::Instance().GetPrimitiveRenderer();
	/*DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMStoreFloat4x4(&transform, S * R * T);*/

	renderer->RenderCylinder(rc, position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
	//primitiveRenderer->DrawSphere(transform, radius, DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
}

//�C���X�^���X�擾
Goal& Goal::Instance() {
	static Goal instance;
	return instance;
}


// �R���X�g���N�^
Goal::Goal()
{
	position = DirectX::XMFLOAT3(19.5f, 0.0f, 19.5f);
}

// �f�X�g���N�^
Goal::~Goal()
{
}

// �`��
void Goal::Render(RenderContext rc, ShapeRenderer* renderer)
{
	// �`��
	//PrimitiveRenderer* primitiveRenderer = Graphics::Instance().GetPrimitiveRenderer();
	/*DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMStoreFloat4x4(&transform, S * R * T);*/

	renderer->RenderCylinder(rc, position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
	//primitiveRenderer->DrawSphere(transform, radius, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
}
