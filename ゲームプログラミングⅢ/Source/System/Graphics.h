#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include "RenderState.h"
#include "ShapeRenderer.h"
#include "ModelRenderer.h"
#include "FrameBuffer.h"
#include "FullScreenquad.h"
#include "GpuResourceUtils.h"
#include "Bloom.h"
#include "imgui.h"

// グラフィックス
class Graphics
{
private:
	Graphics() = default;
	~Graphics() = default;

public:
	// インスタンス取得
	static Graphics& Instance()
	{
		static Graphics instance;
		return instance;
	}

	// 初期化
	void Initialize(HWND hWnd);

	// クリア
	void Clear(float r, float g, float b, float a);

	// レンダーターゲット設定
	void SetRenderTargets();

	// 画面表示
	void Present(UINT syncInterval);

	// ウインドウハンドル取得
	HWND GetWindowHandle() { return hWnd; }

	// デバイス取得
	ID3D11Device* GetDevice() { return device.Get(); }

	// デバイスコンテキスト取得
	ID3D11DeviceContext* GetDeviceContext() { return immediateContext.Get(); }

	// スクリーン幅取得
	float GetScreenWidth() const { return screenWidth; }

	// スクリーン高さ取得
	float GetScreenHeight() const { return screenHeight; }

	// レンダーステート取得
	RenderState* GetRenderState() { return renderState.get(); }

	// シェイプレンダラ取得
	ShapeRenderer* GetShapeRenderer() const { return shapeRenderer.get(); }

	// モデルレンダラ取得
	ModelRenderer* GetModelRenderer() const { return modelRenderer.get(); }

private:
	HWND											hWnd = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;
	D3D11_VIEWPORT									viewport;

	float	screenWidth = 0;
	float	screenHeight = 0;

	std::unique_ptr<RenderState>					renderState;
	std::unique_ptr<ShapeRenderer>					shapeRenderer;
	std::unique_ptr<ModelRenderer>					modelRenderer;




private:
	struct TimeCBuffer
	{

		float time;
		float SignalTime;
		float pad[2];
	};
	struct ScreenSizeCBuffer
	{
		float screenWidth;
		float screenHeight;
		float pad[2];
	};
	enum class ConstantBufferType
	{
		TimeCBuffer,
		ScreenSizeCBuffer,
		Count
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> cbuffer[int(ConstantBufferType::Count)];
public:
	//OFFSCREEN_RENDERING
	enum class PPShaderType
	{
		screenquad,
		crt,
		Glitch,
		Sharpen,
		BreathShake,
		TemporalNoise,
		NoSignalFinale,
		VisionBootDown,
		Crackshaft,
		HighLightPass,
		Blur,
		BloomFinal,
		TVNoiseFade,
		GameOver,
		FilmGrainDust,
		FadeToBlack,
		Count
	};
	std::unique_ptr<framebuffer> framebuffers[int(PPShaderType::Count)];
	std::unique_ptr<fullscreen_quad> bit_block_transfer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shaders[int(PPShaderType::Count)];

	// BLOOM
	std::unique_ptr<bloom> bloomer;


	void UpdateConstantBuffer(float Time, float signalTime = 0) {
		TimeCBuffer timeCBuffer;
		timeCBuffer.time = Time;
		timeCBuffer.SignalTime = signalTime; //SignalTimeを更新するとテレビのような画面切り替え(0.3ｓ)
		immediateContext->UpdateSubresource(cbuffer[int(ConstantBufferType::TimeCBuffer)].Get(), 0, 0, &timeCBuffer, 0, 0);
		immediateContext->PSSetConstantBuffers(10, 1, cbuffer[int(ConstantBufferType::TimeCBuffer)].GetAddressOf());
		immediateContext->VSSetConstantBuffers(10, 1, cbuffer[int(ConstantBufferType::TimeCBuffer)].GetAddressOf());

		ScreenSizeCBuffer screenSizeCBuffer;
		screenSizeCBuffer.screenWidth = screenWidth;
		screenSizeCBuffer.screenHeight = screenHeight;
		immediateContext->UpdateSubresource(cbuffer[int(ConstantBufferType::ScreenSizeCBuffer)].Get(), 0, 0, &screenSizeCBuffer, 0, 0);
		immediateContext->PSSetConstantBuffers(11, 1, cbuffer[int(ConstantBufferType::ScreenSizeCBuffer)].GetAddressOf());
		immediateContext->VSSetConstantBuffers(11, 1, cbuffer[int(ConstantBufferType::ScreenSizeCBuffer)].GetAddressOf());

	}

	void DebugGUI()
	{
		if (ImGui::TreeNode("Bloom"))
		{

			// BLOOM
			ImGui::SliderFloat("bloom_extraction_threshold", &bloomer->bloom_extraction_threshold, +0.0f, +5.0f);
			ImGui::SliderFloat("bloom_intensity", &bloomer->bloom_intensity, +0.0f, +5.0f);

			ImGui::TreePop();
		}
	}
};
