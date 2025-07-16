#include "ObjectManager.h"
#include "Object.h"

#include <imgui.h>

void ObjectManager::Initialize()
{
	/// モデルの読み込み
	models[static_cast<int>(ObjectLavel::SmallChair)] = std::make_unique<Model>("Data/Model/Chair/Chair_B.mdl");
	models[static_cast<int>(ObjectLavel::LargeChair)] = std::make_unique<Model>("Data/Model/Chair/Chair_A.mdl");
	models[static_cast<int>(ObjectLavel::Counter)] = std::make_unique<Model>("Data/Model/Counter/Counter.mdl");
	models[static_cast<int>(ObjectLavel::Table)] = std::make_unique<Model>("Data/Model/Table/Table.mdl");
	models[static_cast<int>(ObjectLavel::TV_A)] = std::make_unique<Model>("Data/Model/tv_assets/tv_A/tv_A.mdl");
	models[static_cast<int>(ObjectLavel::TV_B)] = std::make_unique<Model>("Data/Model/tv_assets/tv_B/tv_B.mdl");

	/// テクスチャの読み込み
	textures[static_cast<int>(ObjectLavel::SmallChair)] = std::make_unique<LoadTextures>();
	textures[static_cast<int>(ObjectLavel::SmallChair)]->LoadNormal("Data/Model/Chair/Chair_mtl/Chair_mtl_Normal.1001.png");
	textures[static_cast<int>(ObjectLavel::SmallChair)]->LoadRoughness("Data/Model/Chair/Chair_mtl/Chair_mtl_Roughness.1001.png");

	textures[static_cast<int>(ObjectLavel::LargeChair)] = std::make_unique<LoadTextures>();
	textures[static_cast<int>(ObjectLavel::LargeChair)]->LoadNormal("Data/Model/Chair/Chair_mtl/Chair_mtl_Normal.1001.png");
	textures[static_cast<int>(ObjectLavel::LargeChair)]->LoadRoughness("Data/Model/Chair/Chair_mtl/Chair_mtl_Roughness.1001.png");

	textures[static_cast<int>(ObjectLavel::Counter)] = std::make_unique<LoadTextures>();
	textures[static_cast<int>(ObjectLavel::Counter)]->LoadNormal("Data/Model/Counter/Counter_mtl/standardSurface1_Normal.1001.png");
	textures[static_cast<int>(ObjectLavel::Counter)]->LoadRoughness("Data/Model/Counter/Counter_mtl/standardSurface1_Roughness.1001.png");
	textures[static_cast<int>(ObjectLavel::Counter)]->LoadMetalness("Data/Model/Counter/Counter_mtl/standardSurface1_Metalness.1001.png");

	textures[static_cast<int>(ObjectLavel::Table)] = std::make_unique<LoadTextures>();
	textures[static_cast<int>(ObjectLavel::Table)]->LoadNormal("Data/Model/Table/Table_mtl/Table_mtl_Normal.1001.png");
	textures[static_cast<int>(ObjectLavel::Table)]->LoadRoughness("Data/Model/Table/Table_mtl/Table_mtl_Roughness.1001.png");

	textures[static_cast<int>(ObjectLavel::TV_A)] = std::make_unique<LoadTextures>();
	textures[static_cast<int>(ObjectLavel::TV_A)]->LoadEmisive("Data/Model/tv_assets/tv_A/textures/tv_A_basecolor_emissive.png");

	textures[static_cast<int>(ObjectLavel::TV_B)] = std::make_unique<LoadTextures>();
	textures[static_cast<int>(ObjectLavel::TV_B)]->LoadNormal("Data/Model/tv_assets/tv_B/textures/tv_B_Normal_DirectX.png");
	textures[static_cast<int>(ObjectLavel::TV_B)]->LoadRoughness("Data/Model/tv_assets/tv_B/textures/tv_B_Roughness.png");

	/// オブジェクトの登録
	objects[static_cast<int>(ObjectLavel::SmallChair)].clear();
	objects[static_cast<int>(ObjectLavel::SmallChair)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{  9.5f, 0, 15.5f }, DirectX::XMFLOAT3{ 0, DirectX::XMConvertToRadians(-90), 0 }));
	objects[static_cast<int>(ObjectLavel::SmallChair)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{  9.5f, 0, 14.7f }, DirectX::XMFLOAT3{ 0, DirectX::XMConvertToRadians(-90), 0 }));
	objects[static_cast<int>(ObjectLavel::SmallChair)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 13.5f, 0,-10.0f }, DirectX::XMFLOAT3{ 0, 0, 0 }));
	objects[static_cast<int>(ObjectLavel::SmallChair)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 13.5f, 0,-10.8f }, DirectX::XMFLOAT3{ 0, 0, 0 }));

	objects[static_cast<int>(ObjectLavel::LargeChair)].clear();
	objects[static_cast<int>(ObjectLavel::LargeChair)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ -9.5f, 0,  15.0f }, DirectX::XMFLOAT3{ 0, DirectX::XMConvertToRadians(-90), 0 }));
	objects[static_cast<int>(ObjectLavel::LargeChair)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ -22.5f, 0, -3.0f }, DirectX::XMFLOAT3{ 0, DirectX::XMConvertToRadians(-90), 0 }));
	objects[static_cast<int>(ObjectLavel::LargeChair)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ -22.5f, 0, -5.0f }, DirectX::XMFLOAT3{ 0, DirectX::XMConvertToRadians(-90), 0 }));
	objects[static_cast<int>(ObjectLavel::LargeChair)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ -14.5f, 0,-10.5f }, DirectX::XMFLOAT3{ 0, 0, 0 }));

	objects[static_cast<int>(ObjectLavel::Counter)].clear();
	objects[static_cast<int>(ObjectLavel::Counter)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{  0.0f, 0, -5.3f }, DirectX::XMFLOAT3{ 0, DirectX::XMConvertToRadians(180), 0 }));
	objects[static_cast<int>(ObjectLavel::Counter)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 1.0f, 0, -24.3f }, DirectX::XMFLOAT3{ 0, DirectX::XMConvertToRadians(180), 0 }));

	objects[static_cast<int>(ObjectLavel::Table)].clear();
	objects[static_cast<int>(ObjectLavel::Table)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ -1.0f, 0, 24.0f }, DirectX::XMFLOAT3{ 0, DirectX::XMConvertToRadians(90), 0 }));
	objects[static_cast<int>(ObjectLavel::Table)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 2.0f, 0, 24.0f }, DirectX::XMFLOAT3{ 0, DirectX::XMConvertToRadians(90), 0 }));
	objects[static_cast<int>(ObjectLavel::Table)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ -1.0f, 0, 20.0f }, DirectX::XMFLOAT3{ 0, DirectX::XMConvertToRadians(90), 0 }));
	objects[static_cast<int>(ObjectLavel::Table)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 2.0f, 0, 20.0f }, DirectX::XMFLOAT3{ 0, DirectX::XMConvertToRadians(90), 0 }));
	objects[static_cast<int>(ObjectLavel::Table)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 5.0f, 0, 2.5f }, DirectX::XMFLOAT3{ 0, DirectX::XMConvertToRadians(90), 0 }));
	objects[static_cast<int>(ObjectLavel::Table)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ 8.0f, 0, 2.5f }, DirectX::XMFLOAT3{ 0, DirectX::XMConvertToRadians(90), 0 }));

	objects[static_cast<int>(ObjectLavel::TV_A)].clear();
	objects[static_cast<int>(ObjectLavel::TV_A)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ -10.5f,1.5f,2.5f }, DirectX::XMFLOAT3{ 0, DirectX::XMConvertToRadians(180), 0 }));

	objects[static_cast<int>(ObjectLavel::TV_B)].clear();
	objects[static_cast<int>(ObjectLavel::TV_B)].push_back(std::make_unique<Object>(DirectX::XMFLOAT3{ -10.5f,1.5f,2.5f }, DirectX::XMFLOAT3{ 0, DirectX::XMConvertToRadians(180), 0 }));
}

void ObjectManager::Update(float elapsedTime)
{
	for (int i = 0; i < OBJ_MAX; ++i) {
		for (auto& p : objects[i]) {
			p->Update(elapsedTime);
		}
	}
}

void ObjectManager::Render(RenderContext& rc, ModelRenderer* renderer)
{
	for (int i = 0; i < OBJ_MAX; ++i) {
		for (auto& p : objects[i]) {
			p->Render(rc, renderer, models[i].get(), textures[i].get());
		}
	}
}

void ObjectManager::RenderDebugPrimitive(RenderContext& rc, ShapeRenderer* renderer)
{
	for (auto& p : objects[static_cast<int>(ObjectLavel::SmallChair)]) {
		renderer->RenderBox(rc, p->GetPosition(), p->GetAngle(), {0.5f,0.5f,0.5f}, {1,0,0,1});
	}
	for (auto& p : objects[static_cast<int>(ObjectLavel::LargeChair)]) {
		renderer->RenderBox(rc, p->GetPosition(), p->GetAngle(), { 1.0f,1.0f,0.5f }, { 1,0,0,1 });
	}
	for (auto& p : objects[static_cast<int>(ObjectLavel::Counter)]) {
		renderer->RenderBox(rc, p->GetPosition(), p->GetAngle(), { 1.0f,1.0f,0.5f }, { 1,0,0,1 });
	}
	for (auto& p : objects[static_cast<int>(ObjectLavel::Table)]) {
		renderer->RenderBox(rc, p->GetPosition(), p->GetAngle(), { 1.0f,1.0f,0.5f }, { 1,0,0,1 });
	}
}

void ObjectManager::DebugGUI()
{
	// 共通処理をラムダ関数にまとめる
	auto drawObjectGUI = [](const char* label, std::vector<std::unique_ptr<Object>>& objects) {
		if (ImGui::TreeNode(label)) {
			for (int i = 0; i < objects.size(); ++i) {
				ImGui::PushID(i); // IDを一意にする

				DirectX::XMFLOAT3 position = objects[i]->GetPosition();
				float angle = DirectX::XMConvertToDegrees(objects[i]->GetAngle().y); // ラジアン→度に変換

				std::string posLabel = "Position##" + std::to_string(i);
				std::string angleLabel = "Angle##" + std::to_string(i);

				if (ImGui::DragFloat3(posLabel.c_str(), &position.x, 0.1f, -30.0f, 30.0f)) {
					objects[i]->SetPosition(position);
				}
				if (ImGui::DragFloat(angleLabel.c_str(), &angle, 0.1f, -180.0f, 180.0f)) {
					objects[i]->SetAngle({ 0, DirectX::XMConvertToRadians(angle), 0 });
				}

				ImGui::PopID(); // IDを戻す
			}
			ImGui::TreePop();
		}
	};

	// 各オブジェクトに対してGUIを描画
	drawObjectGUI("smallChair", objects[static_cast<int>(ObjectLavel::SmallChair)]);
	drawObjectGUI("largeChair", objects[static_cast<int>(ObjectLavel::LargeChair)]);
	drawObjectGUI("counter", objects[static_cast<int>(ObjectLavel::Counter)]);
	drawObjectGUI("table", objects[static_cast<int>(ObjectLavel::Table)]);
	drawObjectGUI("tv_A", objects[static_cast<int>(ObjectLavel::TV_A)]);
	drawObjectGUI("tv_B", objects[static_cast<int>(ObjectLavel::TV_B)]);
}

