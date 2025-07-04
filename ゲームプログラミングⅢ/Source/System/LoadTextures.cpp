#include "LoadTextures.h"

#include "System/Misc.h"
#include "System/Graphics.h"
#include "System/GpuResourceUtils.h"
#include "System/RenderContext.h"
#include "LoadTextures.h"

#include <imgui.h>
#include "Misc.h"

LoadTextures::LoadTextures()
{
}

void LoadTextures::Set(const RenderContext& rc)
{
    ID3D11DeviceContext* dc = rc.deviceContext;

    CreateDummyNormalTexture(dummySRV.GetAddressOf());

    // SRVîzóÒÇçÏê¨
    ID3D11ShaderResourceView* srvs[4];
    for (int i = 0; i < _countof(shaderResourceView); ++i) {
        srvs[i] = shaderResourceView[i].Get();
    }

    if (!shaderResourceView[0])
    {
        shaderResourceView[0] = dummySRV.Get();
    }

    dc->PSSetShaderResources(1, 4, srvs);

}

void LoadTextures::LoadNormal(const char* filename)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();
    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC desc;
    //hr = GpuResourceUtils::LoadTexture(device, filename, normalSRV.GetAddressOf(), &desc);
    hr = GpuResourceUtils::LoadTexture(device, filename, shaderResourceView[0].GetAddressOf(), &desc);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

void LoadTextures::LoadRoughness(const char* filename)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();
    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC desc;
    hr = GpuResourceUtils::LoadTexture(device, filename, shaderResourceView[1].GetAddressOf(), &desc);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

void LoadTextures::LoadMetalness(const char* filename)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();
    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC desc;
    hr = GpuResourceUtils::LoadTexture(device, filename, shaderResourceView[2].GetAddressOf(), &desc);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

void LoadTextures::LoadEmisive(const char* filename)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();
    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC desc;
    hr = GpuResourceUtils::LoadTexture(device, filename, shaderResourceView[3].GetAddressOf(), &desc);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

void LoadTextures::Clear(const RenderContext& rc)
{
    ID3D11DeviceContext* dc = rc.deviceContext;
    ID3D11ShaderResourceView* clearShaderResourceView[] = { nullptr };
    //dc->PSGetShaderResources(1, 4, clearShaderResourceView);
    dc->PSGetShaderResources(1, 1, clearShaderResourceView);
    dc->PSGetShaderResources(2, 1, clearShaderResourceView);
    dc->PSGetShaderResources(2, 1, clearShaderResourceView);
    dc->PSGetShaderResources(4, 1, clearShaderResourceView);
}

HRESULT LoadTextures::CreateDummyNormalTexture(ID3D11ShaderResourceView** shaderResourceView)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();
    HRESULT hr = S_OK;

    float dimension = 16;

    D3D11_TEXTURE2D_DESC texture2dDesc = {};
    texture2dDesc.Width = dimension;
    texture2dDesc.Height = dimension;
    texture2dDesc.MipLevels = 1;
    texture2dDesc.ArraySize = 1;
    texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture2dDesc.SampleDesc.Count = 1;
    texture2dDesc.SampleDesc.Quality = 0;
    texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
    texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    size_t texels = dimension * dimension;
    std::unique_ptr<DWORD[]> sysmem = { std::make_unique<DWORD[]>(texels) };
    for (size_t i = 0; i < texels; ++i) { sysmem[i] = 0xFFFF7F7F; }

    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = sysmem.get();
    subresourceData.SysMemPitch = sizeof(DWORD) * dimension;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
    hr = device->CreateTexture2D(&texture2dDesc, &subresourceData, &texture2d);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
    shaderResourceViewDesc.Format = texture2dDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc, shaderResourceView);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    return hr;
}
