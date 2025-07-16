#include "TV.h"
#include <imgui.h>

void TV::Initialize()
{
	// ���f���̓ǂݍ���
	model[0] = std::make_unique<Model>("Data/Model/tv_assets/tv_A/tv_A.mdl");
	model[1] = std::make_unique<Model>("Data/Model/tv_assets/tv_B/tv_B.mdl");

	// �e�N�X�`���̓ǂݍ���
	textures[0] = std::make_unique<LoadTextures>();
	textures[1] = std::make_unique<LoadTextures>();

	textures[0]->LoadEmisive("Data/Model/tv_assets/tv_A/textures/tv_A_basecolor_emissive.png");
	textures[1]->LoadNormal("Data/Model/tv_assets/tv_B/textures/tv_B_Normal_DirectX.png");
	textures[1]->LoadRoughness("Data/Model/tv_assets/tv_B/textures/tv_B_Roughness.png");

	// �ݒ�
	position = { -10.5f,1.5f,2.5f };
	angle = { 0,DirectX::XMConvertToRadians(180),0 };
	scale = { 0.01f,0.01f,0.01f };
}

void TV::Update(float elapsedTime)
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

	ImGui::InputFloat3("position", &position.x);
	ImGui::InputFloat3("angle", &angle.x);
}

void TV::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (int i = 0; i < _countof(model); ++i) {
		textures[i]->Set(rc);
		renderer->Render(rc, world, model[i].get(), ShaderId::Custom);
		textures[i]->Clear(rc);
	}
}
