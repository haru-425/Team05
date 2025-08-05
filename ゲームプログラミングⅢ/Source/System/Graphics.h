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

// �O���t�B�b�N�X
class Graphics
{
private:
	Graphics() = default;
	~Graphics() = default;

public:
	// �C���X�^���X�擾
	static Graphics& Instance()
	{
		static Graphics instance;
		return instance;
	}

	// ������
	void Initialize(HWND hWnd);

	// �N���A
	void Clear(float r, float g, float b, float a);

	// �����_�[�^�[�Q�b�g�ݒ�
	void SetRenderTargets();

	// ��ʕ\��
	void Present(UINT syncInterval);

	// �E�C���h�E�n���h���擾
	HWND GetWindowHandle() { return hWnd; }

	// �f�o�C�X�擾
	ID3D11Device* GetDevice() { return device.Get(); }

	// �f�o�C�X�R���e�L�X�g�擾
	ID3D11DeviceContext* GetDeviceContext() { return immediateContext.Get(); }

	// �X�N���[�����擾
	float GetScreenWidth() const { return screenWidth; }

	// �X�N���[�������擾
	float GetScreenHeight() const { return screenHeight; }

	// �����_�[�X�e�[�g�擾
	RenderState* GetRenderState() { return renderState.get(); }

	// �V�F�C�v�����_���擾
	ShapeRenderer* GetShapeRenderer() const { return shapeRenderer.get(); }

	// ���f�������_���擾
	ModelRenderer* GetModelRenderer() const { return modelRenderer.get(); }

	BOOL GetScreenMode() const { return screenMode; }

	void OnResize(UINT64 width, UINT height);

	void StylizeWindow(BOOL screenMode = FALSE);

	/// �ύX��̃E�B���h�E�T�C�Y�������E�B���h�E�T�C�Y�̉��{�����擾����֐�
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
		float flickerSpeed; // ��������̃m�C�Y���x�i��F40.0�j
		float flickerStrength; // ���邳�̗h�炬�̋����i��F0.4�j
		float flickerDuration; // �������Ԃ��������ԁi�b�j�i��F1.0�j
		float flickerChance; // ��������N����m���i��F0.05�j
	};
	LightFlickerCBuffer lightFlickerCBuffer = { 40.0f, 0.4f,0.5f,0.1f };
	struct RadialBlurCBuffer
	{
		DirectX::XMFLOAT2 Center; // ���S���W�iUV��ԁA��Ffloat2(0.5, 0.5)�j
		float BlurStrength; // �u���[�̋����i��F0.2�j
		int SampleCount; // �T���v�����i��F16�j
		float FalloffPower; // �t�H�[���I�t�w���i��F2.0�j
		DirectX::XMFLOAT2 DirectionBias; // �����o�C�A�X�i��Ffloat2(1.0, 0.5)�j
		float padding; // �p�f�B���O
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
		timeCBuffer.SignalTime = signalTime; //SignalTime���X�V����ƃe���r�̂悤�ȉ�ʐ؂�ւ�(0.3��)
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

		// �t���b�J�[���ʂ̒萔�o�b�t�@���X�V

		immediateContext->UpdateSubresource(cbuffer[int(ConstantBufferType::LightFlickerCBuffer)].Get(), 0, 0, &lightFlickerCBuffer, 0, 0);
		immediateContext->PSSetConstantBuffers(13, 1, cbuffer[int(ConstantBufferType::LightFlickerCBuffer)].GetAddressOf());
		immediateContext->VSSetConstantBuffers(13, 1, cbuffer[int(ConstantBufferType::LightFlickerCBuffer)].GetAddressOf());
		// ���W�A���u���[�̒萔�o�b�t�@���X�V
		immediateContext->UpdateSubresource(cbuffer[int(ConstantBufferType::RadialBlurCBuffer)].Get(), 0, 0, &radialBlurCBuffer, 0, 0);
		immediateContext->PSSetConstantBuffers(9, 1, cbuffer[int(ConstantBufferType::RadialBlurCBuffer)].GetAddressOf());





	}
	void ShowRadialBlurCBufferUI()
	{
		RadialBlurCBuffer* buffer = &radialBlurCBuffer;

		// ���Z�b�g�{�^��
		if (ImGui::Button("Reset to Default"))
		{
			buffer->Center = { 0.5f, 0.5f };
			buffer->BlurStrength = 0.2f;
			buffer->SampleCount = 16;
			buffer->FalloffPower = 2.0f;
			buffer->DirectionBias = { 1.0f, 1.0f };
			buffer->padding = 0.0f;
		}

		// Center�iUV��ԁj
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
