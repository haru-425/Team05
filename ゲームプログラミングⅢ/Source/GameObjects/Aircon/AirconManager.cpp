#include "AirconManager.h"
#include "System/Graphics.h"
#include <imgui.h>

void AirconManager::Initialize()
{
	// モデルの初期化
	aircons.clear();

	/* 最奥部屋 */
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{ -8,LIGHT_HEIGHT,22 }));
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{  8,LIGHT_HEIGHT,22 }));

	/* 北廊下 */
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{ -21,LIGHT_HEIGHT,13 }));
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{   0,LIGHT_HEIGHT,13 }));
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{  21,LIGHT_HEIGHT,13 }));

	/* 大廊下 */
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{ -21,LIGHT_HEIGHT,5 }));
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{   0,LIGHT_HEIGHT,5 }));
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{  21,LIGHT_HEIGHT,5 }));

	/* 大ライト通路 */
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{ -21,LIGHT_HEIGHT,-13 }));
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{  21,LIGHT_HEIGHT,-13 }));

	/* 入口 */
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{ -21,LIGHT_HEIGHT,-23 }));
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{  -5,LIGHT_HEIGHT,-23 }));
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{  21,LIGHT_HEIGHT,-23 }));
}

void AirconManager::Update()
{

}

void AirconManager::Render(RenderContext& rc)
{
	Graphics& graphics = Graphics::Instance(); ///< グラフィックス管理インスタンス
	ID3D11DeviceContext* dc = graphics.GetDeviceContext(); ///< デバイスコンテキスト
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();

	for (auto& p : aircons) {
		p->Render(rc,modelRenderer);
	}
}

void AirconManager::RenderDebugPrimitive(RenderContext& rc)
{
	Graphics& graphics = Graphics::Instance(); ///< グラフィックス管理インスタンス
	ID3D11DeviceContext* dc = graphics.GetDeviceContext(); ///< デバイスコンテキスト
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();

	for (auto& p : aircons) {
		p->RenderDebugPrimitive(rc, shapeRenderer);
	}
}

void AirconManager::DebugGUI()
{
	if (ImGui::TreeNode("aircon"))
	{

		for (int i = 0; i < AIRCON_MAX; ++i) {
			DirectX::XMFLOAT3 position = aircons.at(i)->GetPosition();

			std::string p = std::string("position") + std::to_string(i);
			ImGui::DragFloat3(p.c_str(), &position.x, 0.1f, -30.0f, +30.0f);
			aircons.at(i)->SetPosition(position);
		}

		ImGui::TreePop();
	}
}
