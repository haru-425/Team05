#include "ShadowCaster.h"
#include "Misc.h"
#include <imgui.h>

ShadowCaster::ShadowCaster(ID3D11Device* device, uint32_t w, uint32_t h)
{

	HRESULT hr = S_OK;

	// バッファ生成
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBuffer{};
	D3D11_TEXTURE2D_DESC texture2dDesc{};
	texture2dDesc.Width = w;
	texture2dDesc.Height = h;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags = 0;
	texture2dDesc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2dDesc, NULL, depthBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	//	深度ステンシルビュー生成
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(depthBuffer.Get(),
		&depthStencilViewDesc,
		shadowMapDepthStencilView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	//	シェーダーリソースビュー生成
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(depthBuffer.Get(),
		&shaderResourceViewDesc,
		shadowShaderResourceView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(w);
	viewport.Height = static_cast<float>(h);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
}

void ShadowCaster::Active(ID3D11DeviceContext* dc)
{
	viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	dc->RSGetViewports(&viewportCount, cachedViewports);
	dc->OMGetRenderTargets(1, cacheRenderTargetView.GetAddressOf(), cacheDepthStencilView.GetAddressOf());

	dc->RSSetViewports(1, &viewport);
	dc->OMSetRenderTargets(0, nullptr, shadowMapDepthStencilView.Get());
}

void ShadowCaster::Deactive(ID3D11DeviceContext* dc)
{
	dc->RSSetViewports(viewportCount, cachedViewports);
	dc->OMSetRenderTargets(1, cacheRenderTargetView.GetAddressOf(), cacheDepthStencilView.Get());
	dc->PSSetShaderResources(8, 1, shadowShaderResourceView.GetAddressOf());
}

void ShadowCaster::Release(ID3D11DeviceContext* dc)
{
	ID3D11ShaderResourceView* clearShaderResourceView[] = { nullptr };
	dc->PSGetShaderResources(8, 1, clearShaderResourceView);
}

void ShadowCaster::DrawGUI()
{
	RenderContext rc;
	DirectX::XMFLOAT3 color = rc.shadowColor;
	float bias = rc.shadowBias;

	ImGui::Separator();

	if (ImGui::TreeNode("texture"))
	{
		ImGui::Text("shadow_map");
		ImGui::Image(shadowShaderResourceView.Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		//ImGui::DragFloat("shadowBias", &bias, 0.0001f, 0, 1, "%.6f");
		//ImGui::ColorEdit3("shadowColor", reinterpret_cast<float*>(&color));

		ImGui::TreePop();
	}
}

void ShadowCaster::Clear(ID3D11DeviceContext* dc, float depth)
{
	dc->ClearDepthStencilView(shadowMapDepthStencilView.Get(), D3D11_CLEAR_DEPTH, depth, 0);
}
