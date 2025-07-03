#include "LoadTextures.h"

#include "System/Misc.h"
#include "System/Graphics.h"
#include "System/GpuResourceUtils.h"
#include "System/RenderContext.h"
#include "LoadTextures.h"

LoadTextures::LoadTextures()
{
}

void LoadTextures::Set(const RenderContext& rc)
{
    ID3D11DeviceContext* dc = rc.deviceContext;
    ID3D11ShaderResourceView* texArraySRVs[4] = {
        normalSRV.Get(),roughnessSRV.Get(),metalnessSRV.Get(),emisiveSRV.Get()
    };
    dc->PSSetShaderResources(1, 4, texArraySRVs);
}

void LoadTextures::LoadNormal(const char* filename)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();
    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC desc;
    hr = GpuResourceUtils::LoadTexture(device, filename, normalSRV.GetAddressOf(), &desc);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

void LoadTextures::LoadRoughness(const char* filename)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();
    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC desc;
    hr = GpuResourceUtils::LoadTexture(device, filename, roughnessSRV.GetAddressOf(), &desc);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

void LoadTextures::LoadMetalness(const char* filename)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();
    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC desc;
    hr = GpuResourceUtils::LoadTexture(device, filename, metalnessSRV.GetAddressOf(), &desc);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

void LoadTextures::LoadEmisive(const char* filename)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();
    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC desc;
    hr = GpuResourceUtils::LoadTexture(device, filename, emisiveSRV.GetAddressOf(), &desc);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

void LoadTextures::Clear(const RenderContext& rc)
{
    ID3D11DeviceContext* dc = rc.deviceContext;
    ID3D11ShaderResourceView* clearShaderResourceView[] = { nullptr };
    dc->PSGetShaderResources(1, 1, clearShaderResourceView);
    dc->PSGetShaderResources(2, 1, clearShaderResourceView);
}
