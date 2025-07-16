#include "ObjectManager.h"

void ObjectManager::Initialize()
{
	/// モデルの読み込み
	models[0] = std::make_unique<Model>("Data/Model/Chair/Chair_A.mdl");
	models[1] = std::make_unique<Model>("Data/Model/Chair/Chair_B.mdl");
	models[2] = std::make_unique<Model>("Data/Model/Counter/Counter.mdl");
	models[3] = std::make_unique<Model>("Data/Model/Table/Table.mdl");

	/// テクスチャの読み込み
	textures[0] = std::make_unique<LoadTextures>();
	textures[0]->LoadNormal("Data/Model/Chair/Chair_mtl/Chair_mtl_Normal.1001.png");
	textures[0]->LoadRoughness("Data/Model/Chair/Chair_mtl/Chair_mtl_Roughness.1001.png");

	textures[1] = std::make_unique<LoadTextures>();
	textures[1]->LoadNormal("Data/Model/Chair/Counter_mtl/standardSurface1_Normal.1001.png");
	textures[1]->LoadRoughness("Data/Model/Chair/Counter_mtl/standardSurface1_Roughness.1001.png");
	textures[1]->LoadMetalness("Data/Model/Chair/Counter_mtl/standardSurface1_Metalness.1001.png");

	textures[2] = std::make_unique<LoadTextures>();
	textures[2]->LoadNormal("Data/Model/Table/Table_mtl/Table_mtl_Normal.1001.png");
	textures[2]->LoadRoughness("Data/Model/Table/Table_mtl/Table_mtl_Roughness.1001.png");

	/// オブジェクトの登録
	//smallChair.clear();
	smallChair.emplace_back(Object(DirectX::XMFLOAT3{ 0, 0, 0 }, { models[0].get() }, { textures[0].get()}));

	//smallChair.clear();
	//smallChair.push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 0, 0, 0 }));
	//smallChair.push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 0, 0, 0 }));
	//smallChair.push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 0, 0, 0 }));
	//smallChair.push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 0, 0, 0 }));

	//largeChair.clear();
	//largeChair.push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 0, 0, 0 }));
	//largeChair.push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 0, 0, 0 }));
	//largeChair.push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 0, 0, 0 }));
	//largeChair.push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 0, 0, 0 }));

	//counter.clear();
	//counter.push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 0, 0, 0 }));
	//counter.push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 0, 0, 0 }));

	//table.clear();
	//table.push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 0, 0, 0 }));
	//table.push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 0, 0, 0 }));
	//table.push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 0, 0, 0 }));
	//table.push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 0, 0, 0 }));
	//table.push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 0, 0, 0 }));
	//table.push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 0, 0, 0 }));
}

void ObjectManager::update()
{

}

void ObjectManager::Render(RenderContext& rc, ModelRenderer* renderer)
{
	/*for (auto& p : smallChair) {
		p->Render(rc, renderer, models[0].get(), textures[0].get());
	}
	for (auto& p : largeChair) {
		p->Render(rc, renderer, models[1].get(), textures[0].get());
	}
	for (auto& p : counter) {
		p->Render(rc, renderer, models[2].get(), textures[1].get());
	}
	for (auto& p : table) {
		p->Render(rc, renderer, models[3].get(), textures[2].get());
	}*/
}

void ObjectManager::RenderDebugPrimitive(RenderContext& rc, ShapeRenderer* renderer)
{
	/*for (auto& p : smallChair) {	
		renderer->RenderBox(rc, p->GetPosition(), { 0,0,0 }, { 0.5f,0.5f,0.5f }, { 1,0,0,1 });
	}
	for (auto& p : largeChair) {
		renderer->RenderBox(rc, p->GetPosition(), { 0,0,0 }, { 1.0f,1.0f,1.0f }, { 1,0,0,1 });
	}
	for (auto& p : counter) {
		renderer->RenderBox(rc, p->GetPosition(), { 0,0,0 }, { 1.0f,1.0f,0.5f }, { 1,0,0,1 });
	}
	for (auto& p : counter) {
		renderer->RenderBox(rc, p->GetPosition(), { 0,0,0 }, { 1.0f,1.0f,0.5f }, { 1,0,0,1 });
	}*/
}
