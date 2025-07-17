#include "Object/Object.h"

#include <imgui.h>

Object::Object(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 angle, DirectX::XMFLOAT3 scale) :position(position), angle(angle), scale(scale)
{
	//scale = { 0.01f,0.01f,0.01f };

	//スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//回転行列
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//３つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;
	//計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&world, W);
}

void Object::Update(float elapsedTime)
{
	//スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.x);
	//回転行列
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//３つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;
	//計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&world, W);
}

void Object::Render(const RenderContext& rc, ModelRenderer* renderer, Model* model, LoadTextures* textures)
{
	textures->Set(rc);
	renderer->Render(rc, world, model, ShaderId::Custom);
	textures->Clear(rc);
}

void Object::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
}

void Object::DebugGUI()
{
	float debugAngle = DirectX::XMConvertToDegrees(angle.y);
	ImGui::DragFloat3("position", &position.x, 0.1f, -30, 30);
	ImGui::DragFloat("angle", &debugAngle, 0.1f, -180, 180);

	angle.y = DirectX::XMConvertToRadians(debugAngle);
}
