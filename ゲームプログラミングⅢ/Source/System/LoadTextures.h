#pragma once

#include "System/ModelRenderer.h"
#include <memory>
#include <wrl.h>

class LoadTextures
{
public:
	LoadTextures();
	virtual ~LoadTextures() {}

	void Set(const RenderContext& rc);
	void LoadNormal(const char* filename);
	void LoadRoughness(const char* filename);
	void LoadMetalness(const char* filename);
	void LoadEmisive(const char* filename);

	void Clear(const RenderContext& rc);

private:
	HRESULT CreateDummyNormalTexture(ID3D11ShaderResourceView** shaderResourceView);

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughnessSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalnessSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> emisiveSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dummySRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView[4];
};

