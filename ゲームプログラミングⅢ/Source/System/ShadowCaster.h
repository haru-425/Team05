#pragma once

#include <memory>
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Model.h"
#include "Shader.h"

class ShadowCaster
{
public:

	ShadowCaster(ID3D11Device* device,uint32_t width, uint32_t height );
	virtual~ShadowCaster() = default;
	ShadowCaster(const ShadowCaster&) = delete;
	ShadowCaster& operator=(const ShadowCaster&) = delete;
	ShadowCaster(ShadowCaster&&) = delete;
	ShadowCaster& operator=(ShadowCaster&&)noexcept = delete;


	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowShaderResourceView;
	D3D11_VIEWPORT viewport;

	void Clear(ID3D11DeviceContext* dc, float depth);

	void Active(ID3D11DeviceContext* dc);
	void Deactive(ID3D11DeviceContext* dc);

	void Release(ID3D11DeviceContext* dc);

	void DrawGUI();
	
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   shadowMapDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>       shadowMapSamplerState;

private:
	UINT viewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	 cacheRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   cacheDepthStencilView;

};
