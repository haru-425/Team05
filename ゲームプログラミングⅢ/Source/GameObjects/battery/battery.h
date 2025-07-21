#pragma once
#include "GameObject.h"
#include <DirectXMath.h>
#include <vector>
#include "System/LoadTextures.h"

#include "imgui.h"                    // ImGui�̊�{�@�\
#include "imgui_impl_win32.h"        // Win32�p�o�b�N�G���h
#include "imgui_impl_dx11.h"         // DirectX11�p�o�b�N�G���h

#include <imgui.h>

#define RATESPEED 5

class battery:public GameObject
{
public:
	battery();
	~battery();
	void Update(float elapsedTime) override;
	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

	DirectX::XMFLOAT3 getPos() { return position; }

	void setPos(DirectX::XMFLOAT3 pos) { position = pos; }

	void setModel(std::shared_ptr<Model> model) { this->model = model; }

	void ImGUI()
	{
		ImGui::Begin("Battry Info");

		// position��\��
		ImGui::SliderFloat3("Position",offsets, -10.0f, 10.0f, "%.2f",1);

		ImGui::End();
	}
private:
	std::shared_ptr<Model> model = nullptr;                ///< ���f���f�[�^
	std::shared_ptr<LoadTextures> textures;                /// �e�N�X�`���f�[�^

	float offsets[3] = {0.0f, 0.0f, 0.0f};
	DirectX::XMFLOAT3 offset = { 0.0f, 0.18f, 0.0f };
};