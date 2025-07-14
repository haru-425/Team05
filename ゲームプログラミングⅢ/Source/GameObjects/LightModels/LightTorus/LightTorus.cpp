#include "LightTorus.h"
#include "System/Misc.h"

LightTorus::LightTorus(DirectX::XMFLOAT3 position)
{
	//model = std::make_unique<Model>("Data/Model/LightModels/light_circle_assets/light_circle.mdl");

	Benchmark bench;
	bench.begin();

	//textures = std::make_unique<LoadTextures>();
	//textures->LoadNormal("Data/Model/LightModels/light_circle_assets/textures/light_circle_Normal_DirectX.png");
	//textures->LoadRoughness("Data/Model/LightModels/light_circle_assets/textures/light_circle_Roughness.png");
	//textures->LoadEmisive("Data/Model/LightModels/light_circle_assets/textures/light_circle_Emissive.png");

	float timer = bench.end();
	char buffer[256];
	sprintf_s(buffer, "Time taken to load textures in Torus : % f\n", timer);
	OutputDebugStringA(buffer);

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

void LightTorus::Update(float elapsedTime)
{
}

void LightTorus::Render(const RenderContext& rc, ModelRenderer* renderer, Model* model, LoadTextures* texture)
{
	texture->Set(rc);

	//レンダラにモデルを描画してもらう
	renderer->Render(rc, world, model, ShaderId::Custom);

	texture->Clear(rc);
}
