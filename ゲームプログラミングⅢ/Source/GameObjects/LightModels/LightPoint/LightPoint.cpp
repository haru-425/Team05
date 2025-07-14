#include "LightPoint.h"
#include "System/Misc.h"

LightPoint::LightPoint(DirectX::XMFLOAT3 position)
{
	//model = std::make_unique<Model>("Data/Model/LightModels/light_point_assets/light_point.mdl");

	Benchmark bench;
	bench.begin();

	/*textures = std::make_unique<LoadTextures>();
	textures->LoadNormal("Data/Model/LightModels/light_point_assets/textures/light_point_mtl_Normal_DirectX.png");
	textures->LoadRoughness("Data/Model/LightModels/light_point_assets/textures/light_point_mtl_Roughness.png");
	textures->LoadMetalness("Data/Model/LightModels/light_point_assets/textures/light_point_mtl_Metallic.png");
	textures->LoadEmisive("Data/Model/LightModels/light_point_assets/textures/light_point_mtl_Emissive.png");*/

	float timer = bench.end();
	char buffer[256];
	sprintf_s(buffer, "Time taken to load textures in Point : % f\n", timer);
	OutputDebugStringA(buffer);

	this->position = position;

	scale = { 0.005f,0.005f,0.005f };

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

void LightPoint::Update(float elapsedTime)
{
}

void LightPoint::Render(const RenderContext& rc, ModelRenderer* renderer, Model* model, LoadTextures* texture)
{
	texture->Set(rc);

	//レンダラにモデルを描画してもらう
	renderer->Render(rc, world, model, ShaderId::Custom);

	texture->Clear(rc);
}
