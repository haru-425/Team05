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
		DirectX::XMFLOAT2 Center; // ブラー中心（UV）
		float BlurStrength; // ブラーの強さ
		int SampleCount; // サンプル数

		float FalloffPower; // フォールオフ強度（指数）
		DirectX::XMFLOAT2 DirectionBias; // 方向バイアス
		float OffsetStrength; // サンプル開始点を中心からずらす量（0?1）

		float CenterFeather; // 中心ぼかし軽減（0?1）
		float BlendAmount; // 元画像との混合比（0?1）向バイアス（例：float2(1.0, 0.5)）
		float padding[2]; // パディング
	};

	RadialBlurCBuffer radialBlurCBuffer =
	{
		{ 0.5f, 0.5f }, // Center
		0.2f, // BlurStrength
		16, // SampleCount
		2.0f, // FalloffPower
		{ 1.0f, 1.0f }, // DirectionBias
		0.0f, // OffsetStrength
		1.0f, // CenterFeather
		1.0f, // BlendAmount
		{ 0.0f, 0.0f } // padding
	};
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
		VHS,
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


		// デフォルト値（必要なら外部から受け取るように拡張可）
		const RadialBlurCBuffer defaultValues = {
			{0.5f, 0.5f},  // Center
			0.2f,          // BlurStrength
			16,            // SampleCount
			2.0f,          // FalloffPower
			{1.0f, 1.0f},  // DirectionBias
			0.0f,          // OffsetStrength
			0.0f,          // CenterFeather
			1.0f,          // BlendAmount
			{0.0f, 0.0f}   // padding
		};

		if (ImGui::Button("Reset to Default"))
		{
			*buffer = defaultValues;
		}

		// Center (UV)
		float center[2] = { buffer->Center.x, buffer->Center.y };
		if (ImGui::DragFloat2("Center (UV)", center, 0.01f, 0.0f, 1.0f, "%.4f"))
		{
			buffer->Center = { center[0], center[1] };
		}

		ImGui::DragFloat("Blur Strength", &buffer->BlurStrength, 0.01f, 0.0f, 1.0f, "%.4f");
		ImGui::DragInt("Sample Count", &buffer->SampleCount, 1, 1, 128);

		ImGui::DragFloat("Falloff Power", &buffer->FalloffPower, 0.05f, 0.0f, 10.0f, "%.4f");

		float dirBias[2] = { buffer->DirectionBias.x, buffer->DirectionBias.y };
		if (ImGui::DragFloat2("Direction Bias (min 1.0)", dirBias, 0.01f, 1.0f, 10.0f, "%.4f"))
		{
			// 強制的に 1.0 以上に制限
			buffer->DirectionBias.x = max(1.0f, dirBias[0]);
			buffer->DirectionBias.y = max(1.0f, dirBias[1]);
		}

		ImGui::DragFloat("Offset Strength", &buffer->OffsetStrength, 0.01f, 0.0f, 1.0f, "%.4f");
		ImGui::DragFloat("Center Feather", &buffer->CenterFeather, 0.01f, 0.0f, 1.0f, "%.4f");
		ImGui::DragFloat("Blend Amount", &buffer->BlendAmount, 0.01f, 0.0f, 1.0f, "%.4f");
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
