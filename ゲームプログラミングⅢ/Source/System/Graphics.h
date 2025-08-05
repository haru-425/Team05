#pragma once

//#include <d3d11.h>
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
#include <d3d11_1.h>
#include <dxgi1_6.h>

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

	BOOL GetScreenMode() const { return screenMode; }

	void OnResize(UINT64 width, UINT height);

	void StylizeWindow(BOOL screenMode = FALSE);

	/// 変更後のウィンドウサイズが初期ウィンドウサイズの何倍かを取得する関数
	DirectX::XMFLOAT2 GetWindowScaleFactor() const;

private:
	void AcquireHighPerformanceAdapter(IDXGIFactory6* dxgiFactory6, IDXGIAdapter3** dxgiAdapter3);
	void CreateSwapChain(IDXGIFactory6* dxgiFactory6);

private:
	HWND											hWnd = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain1>			swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;
	D3D11_VIEWPORT									viewport;

	float	screenWidth = 0;
	float	screenHeight = 0;

	std::unique_ptr<RenderState>					renderState;
	std::unique_ptr<ShapeRenderer>					shapeRenderer;
	std::unique_ptr<ModelRenderer>					modelRenderer;

	Microsoft::WRL::ComPtr<IDXGIAdapter3> adapter;
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory6;
	SIZE framebufferDimensions = {};
#ifdef _DEBUG
	BOOL screenMode = FALSE;
#else
	BOOL screenMode = TRUE;
#endif
	BOOL tearingSupported = FALSE;
	RECT windowedRect = {};

private:
	struct TimeCBuffer
	{

		float time;
		float SignalTime;
		float remaininngTime;
		float pad;
	};
	struct ScreenSizeCBuffer
	{
		float screenWidth;
		float screenHeight;
		float pad[2];
	};
	struct LightFlickerCBuffer
	{
		float flickerSpeed; // ちらつき中のノイズ速度（例：40.0）
		float flickerStrength; // 明るさの揺らぎの強さ（例：0.4）
		float flickerDuration; // ちらつき状態が続く時間（秒）（例：1.0）
		float flickerChance; // ちらつきが起こる確率（例：0.05）
	};
	LightFlickerCBuffer lightFlickerCBuffer = { 40.0f, 0.4f,0.5f,0.1f };
	struct RadialBlurCBuffer
	{
		DirectX::XMFLOAT2 Center; // 中心座標（UV空間、例：float2(0.5, 0.5)）
		float BlurStrength; // ブラーの強さ（例：0.2）
		int SampleCount; // サンプル数（例：16）
		float FalloffPower; // フォールオフ指数（例：2.0）
		DirectX::XMFLOAT2 DirectionBias; // 方向バイアス（例：float2(1.0, 0.5)）
		float padding; // パディング
	};
	RadialBlurCBuffer radialBlurCBuffer = { {0.5f, 0.5f}, 0.2f, 16, 2.0f, {1.0f, 1.0f}, 0.0f };
	enum class ConstantBufferType
	{
		TimeCBuffer,
		ScreenSizeCBuffer,
		LightFlickerCBuffer,
		RadialBlurCBuffer,
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
		WardenGaze,
		NoiseChange,
		LightFlicker,
		Timer,
		RedPulseAlert,
		RadialBlur,
		Count
	};
	std::unique_ptr<framebuffer> framebuffers[int(PPShaderType::Count)];
	std::unique_ptr<fullscreen_quad> bit_block_transfer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shaders[int(PPShaderType::Count)];

	// BLOOM
	std::unique_ptr<bloom> bloomer;

	void setRadialBlurCBuffer(
		const DirectX::XMFLOAT2& center = { 0.5f, 0.5f },
		float blurStrength = 0.2f,
		int sampleCount = 16,
		float falloffPower = 2.0f,
		const DirectX::XMFLOAT2& directionBias = { 1.0f, 1.0f }
	) {
		radialBlurCBuffer.Center = center;
		radialBlurCBuffer.BlurStrength = blurStrength;
		radialBlurCBuffer.SampleCount = sampleCount;
		radialBlurCBuffer.FalloffPower = falloffPower;
		radialBlurCBuffer.DirectionBias = directionBias;
	}
	void UpdateConstantBuffer(float Time, float signalTime = 0, float remaininngTime = 0) {
		TimeCBuffer timeCBuffer;
		timeCBuffer.time = Time;
		timeCBuffer.SignalTime = signalTime; //SignalTimeを更新するとテレビのような画面切り替え(0.3ｓ)
		timeCBuffer.remaininngTime = remaininngTime;
		immediateContext->UpdateSubresource(cbuffer[int(ConstantBufferType::TimeCBuffer)].Get(), 0, 0, &timeCBuffer, 0, 0);
		immediateContext->PSSetConstantBuffers(10, 1, cbuffer[int(ConstantBufferType::TimeCBuffer)].GetAddressOf());
		immediateContext->VSSetConstantBuffers(10, 1, cbuffer[int(ConstantBufferType::TimeCBuffer)].GetAddressOf());

		ScreenSizeCBuffer screenSizeCBuffer;
		screenSizeCBuffer.screenWidth = screenWidth;
		screenSizeCBuffer.screenHeight = screenHeight;
		immediateContext->UpdateSubresource(cbuffer[int(ConstantBufferType::ScreenSizeCBuffer)].Get(), 0, 0, &screenSizeCBuffer, 0, 0);
		immediateContext->PSSetConstantBuffers(11, 1, cbuffer[int(ConstantBufferType::ScreenSizeCBuffer)].GetAddressOf());
		immediateContext->VSSetConstantBuffers(11, 1, cbuffer[int(ConstantBufferType::ScreenSizeCBuffer)].GetAddressOf());

		// フリッカー効果の定数バッファを更新

		immediateContext->UpdateSubresource(cbuffer[int(ConstantBufferType::LightFlickerCBuffer)].Get(), 0, 0, &lightFlickerCBuffer, 0, 0);
		immediateContext->PSSetConstantBuffers(13, 1, cbuffer[int(ConstantBufferType::LightFlickerCBuffer)].GetAddressOf());
		immediateContext->VSSetConstantBuffers(13, 1, cbuffer[int(ConstantBufferType::LightFlickerCBuffer)].GetAddressOf());
		// ラジアルブラーの定数バッファを更新
		immediateContext->UpdateSubresource(cbuffer[int(ConstantBufferType::RadialBlurCBuffer)].Get(), 0, 0, &radialBlurCBuffer, 0, 0);
		immediateContext->PSSetConstantBuffers(9, 1, cbuffer[int(ConstantBufferType::RadialBlurCBuffer)].GetAddressOf());





	}
	void ShowRadialBlurCBufferUI()
	{
		RadialBlurCBuffer* buffer = &radialBlurCBuffer;

		// リセットボタン
		if (ImGui::Button("Reset to Default"))
		{
			buffer->Center = { 0.5f, 0.5f };
			buffer->BlurStrength = 0.2f;
			buffer->SampleCount = 16;
			buffer->FalloffPower = 2.0f;
			buffer->DirectionBias = { 1.0f, 1.0f };
			buffer->padding = 0.0f;
		}

		// Center（UV空間）
		float center[2] = { buffer->Center.x, buffer->Center.y };
		if (ImGui::DragFloat2("Center (UV 0.0 - 1.0)", center, 0.01f, 0.0f, 1.0f, "%.4f"))
		{
			buffer->Center.x = center[0];
			buffer->Center.y = center[1];
		}

		// Blur Strength
		ImGui::DragFloat("Blur Strength", &buffer->BlurStrength, 0.01f, 0.0f, 1.0f, "%.4f");

		// Sample Count
		ImGui::DragInt("Sample Count", &buffer->SampleCount, 1, 1, 128);
		if (buffer->SampleCount < 1) buffer->SampleCount = 1;

		// Falloff Power
		ImGui::DragFloat("Falloff Power", &buffer->FalloffPower, 0.1f, 0.0f, 10.0f, "%.4f");
		if (buffer->FalloffPower < 0.0f) buffer->FalloffPower = 0.0f;

		// Direction Bias
		float dirBias[2] = { buffer->DirectionBias.x, buffer->DirectionBias.y };
		if (ImGui::DragFloat2("Direction Bias (>= 1.0)", dirBias, 0.01f, 1.0f, 10.0f, "%.4f"))
		{
			buffer->DirectionBias.x = max(1.0f, dirBias[0]);
			buffer->DirectionBias.y = max(1.0f, dirBias[1]);
		}
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
		if (ImGui::TreeNode("Radial Blur"))
		{
			ShowRadialBlurCBufferUI();
			ImGui::TreePop();
		}
	}
};
