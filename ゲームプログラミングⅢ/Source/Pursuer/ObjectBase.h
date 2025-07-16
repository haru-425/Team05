#pragma once
#include <DirectXMath.h>
#include "System/ModelRenderer.h"
#include "System/ShapeRenderer.h"

class ObjectBase
{
public:
	ObjectBase() {}
	virtual ~ObjectBase() {}

	// �s��X�V����
	void UpdateTransform();

	// �ʒu�擾
	const DirectX::XMFLOAT3& GetPosition() const { return position; }

	// �ʒu�ݒ�
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

	// ��]�擾
	const DirectX::XMFLOAT3& GetAngle()const { return angle; }

	// ��]�ݒ�
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	// �X�P�[���擾
	const DirectX::XMFLOAT3& GetScale()const { return scale; }

	// �X�P�[���ݒ�
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	// ���a�擾
	float GetRadius() const { return radius; }

	// �����擾
	float GetHeight()const { return height; };

protected:
	DirectX::XMFLOAT3	position = { 0,0,0 };
	DirectX::XMFLOAT3	angle = { 0,0,0, };
	DirectX::XMFLOAT3	scale = { 1,1,1 };
	DirectX::XMFLOAT4X4	transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	float	radius = 0.3f;
	float	height = 2.0f;

private:
};

// Start
class Start :public ObjectBase
{
public:
	// �R���X�g���N�^
	Start();

	// �f�X�g���N�^
	~Start() override;

	static Start& Instance();

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(RenderContext rc, ShapeRenderer* renderer);

private:
	float				moveSpeed = 5.0f;
};

// �S�[��
class Goal :public ObjectBase
{
public:
	// �R���X�g���N�^
	Goal();

	// �f�X�g���N�^
	~Goal() override;

	static Goal& Instance();

	// �`�揈��
	void Render(RenderContext rc, ShapeRenderer* renderer);
};

