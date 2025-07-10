#include "Aircon.h"

Aircon::Aircon(DirectX::XMFLOAT3 position)
{
	model = std::make_unique<Model>("Data/Model/airCon_assets/airCon_geo.mdl");

	textures = std::make_unique<LoadTextures>();
	textures->LoadNormal("Data/Model/airCon_assets/textures/airCon_mtl_Normal_DirectX.png");
	textures->LoadRoughness("Data/Model/airCon_assets/textures/airCon_mtl_Roughness.png");

	this->position = position;

	scale = { 0.01f,0.01f,0.01f };

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

void Aircon::Update(float elapsedTime)
{
}

void Aircon::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	textures->Set(rc);

	//レンダラにモデルを描画してもらう
	renderer->Render(rc, world, model.get(), ShaderId::Custom);

	textures->Clear(rc);
}

void Aircon::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	renderer->RenderBox(rc, position, angle, { 1,0.5f,1 }, { 0,1,1,1 });
}
