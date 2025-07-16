#include "Object.h"

#include <imgui.h>

//Object::Object(DirectX::XMFLOAT3 position) :position(position)
//{
//	scale = { 0.01f,0.01f,0.01f };
//
//	//�X�P�[���s����쐬
//	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
//	//��]�s��
//	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
//	//�ʒu�s����쐬
//	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
//	//�R�̍s���g�ݍ��킹�A���[���h�s����쐬
//	DirectX::XMMATRIX W = S * R * T;
//	//�v�Z�������[���h�s������o��
//	DirectX::XMStoreFloat4x4(&world, W);
//}

Object::Object(DirectX::XMFLOAT3 position, Model* model, LoadTextures* textures) :model(model), textures(textures)
{
	scale = { 0.01f,0.01f,0.01f };

	//�X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//��]�s��
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//�ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//�R�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;
	//�v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&world, W);
}

void Object::Update(float elapsedTime)
{
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

void Object::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	textures->Set(rc);
	renderer->Render(rc, world, model.get(), ShaderId::Custom);
	textures->Clear(rc);
}

void Object::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
}

void Object::DebugGUI()
{
	ImGui::DragFloat3("position", &position.x);
}
