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
	void LoadOcclusion(const char* filename);

	void Clear(const RenderContext& rc);

private:
	HRESULT CreateDummyTexture(ID3D11ShaderResourceView** shaderResourceView, DWORD value);

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView[5];
};

