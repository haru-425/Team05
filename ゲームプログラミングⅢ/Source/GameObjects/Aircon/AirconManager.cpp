#include "AirconManager.h"
#include "System/Graphics.h"
#include <imgui.h>

void AirconManager::Initialize()
{
	// ���f���̏�����
	aircons.clear();

	/* �ŉ����� */
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{ -8,LIGHT_HEIGHT,22 }));
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{  8,LIGHT_HEIGHT,22 }));

	/* �k�L�� */
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{ -21,LIGHT_HEIGHT,13 }));
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{   0,LIGHT_HEIGHT,13 }));
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{  21,LIGHT_HEIGHT,13 }));

	/* ��L�� */
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{ -21,LIGHT_HEIGHT,5 }));
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{   0,LIGHT_HEIGHT,5 }));
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{  21,LIGHT_HEIGHT,5 }));

	/* �僉�C�g�ʘH */
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{ -21,LIGHT_HEIGHT,-13 }));
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{  21,LIGHT_HEIGHT,-13 }));

	/* ���� */
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{ -21,LIGHT_HEIGHT,-23 }));
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{  -5,LIGHT_HEIGHT,-23 }));
	aircons.push_back(std::make_unique<Aircon>(DirectX::XMFLOAT3{  21,LIGHT_HEIGHT,-23 }));
}

void AirconManager::Update()
{

}

void AirconManager::Render(RenderContext& rc)
{
	Graphics& graphics = Graphics::Instance(); ///< �O���t�B�b�N�X�Ǘ��C���X�^���X
	ID3D11DeviceContext* dc = graphics.GetDeviceContext(); ///< �f�o�C�X�R���e�L�X�g
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();

	for (auto& p : aircons) {
		p->Render(rc,modelRenderer);
	}
}

void AirconManager::RenderDebugPrimitive(RenderContext& rc)
{
	Graphics& graphics = Graphics::Instance(); ///< �O���t�B�b�N�X�Ǘ��C���X�^���X
	ID3D11DeviceContext* dc = graphics.GetDeviceContext(); ///< �f�o�C�X�R���e�L�X�g
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
