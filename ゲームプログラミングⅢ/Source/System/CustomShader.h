#pragma once

#include "Shader.h"

class CustomShader : public Shader
{
public:
	CustomShader(ID3D11Device* device);
	~CustomShader() override = default;

	// 開始処理
	void Begin(const RenderContext& rc) override;

	// 更新処理
	void Update(const RenderContext& rc, const ModelResource::Material& material) override;

	// 終了処理
	void End(const RenderContext& rc) override;

private:
	struct CbMaterial
	{
		DirectX::XMFLOAT4		materialColor;
	};

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffer;
};
