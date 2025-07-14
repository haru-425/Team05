#include "LightBar.h"
#include "System/Misc.h"

LightBar::LightBar(DirectX::XMFLOAT3 position, float angle)
{
	//model = std::make_unique<Model>("Data/Model/LightModels/light_bar_low_assets/light_bar_low.mdl");
	Benchmark bench;
	bench.begin();

	/*textures = std::make_unique<LoadTextures>();
	textures->LoadRoughness("Data/Model/LightModels/light_bar_low_assets/textures/light_bar_low_Roughness.png");
	textures->LoadEmisive("Data/Model/LightModels/light_bar_low_assets/textures/light_bar_low_Emissive.png");*/

	float timer = bench.end();
	char buffer[256];
	sprintf_s(buffer, "Time taken to load textures in Bar : % f\n", timer);
	OutputDebugStringA(buffer);

	this->position = position;
	this->angle.y = angle;

	scale = { 0.01f,0.01f,0.01f };

	//スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.x);
	//回転行列
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(this->angle.x, this->angle.y, this->angle.z);
	//位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//３つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;
	//計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&world, W);
}

LightBar::~LightBar()
{
}

void LightBar::Update(float elapsedTime)
{
}

void LightBar::Render(const RenderContext& rc, ModelRenderer* renderer, Model* model, LoadTextures* texture)
{
	texture->Set(rc);

	//レンダラにモデルを描画してもらう
	renderer->Render(rc, world, model, ShaderId::Custom);

	texture->Clear(rc);
}