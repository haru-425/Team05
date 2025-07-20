#include "Misc.h"
#include "Graphics.h"
#include "Input.h"

// ������
void Graphics::Initialize(HWND hWnd)
{
	this->hWnd = hWnd;
	// ��ʂ̃T�C�Y���擾����B
	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT screenWidth = rc.right - rc.left;
	UINT screenHeight = rc.bottom - rc.top;

	framebufferDimensions.cx = screenWidth;
	framebufferDimensions.cy = screenHeight;

	this->screenWidth = static_cast<float>(screenWidth);
	this->screenHeight = static_cast<float>(screenHeight);

	HRESULT hr = S_OK;

	// �f�o�C�X���X���b�v�`�F�[���̐���
	{
		UINT createFactoryFlags = 0;
#ifdef _DEBUG
		createFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(dxgiFactory6.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		AcquireHighPerformanceAdapter(dxgiFactory6.Get(), adapter.GetAddressOf());

		UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

#if 0
		// �X���b�v�`�F�[�����쐬���邽�߂̐ݒ�I�v�V����
		DXGI_SWAP_CHAIN_DESC swapchainDesc;
		{
			swapchainDesc.BufferDesc.Width = screenWidth;
			swapchainDesc.BufferDesc.Height = screenHeight;
			swapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
			swapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
			swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			swapchainDesc.SampleDesc.Count = 1;
			swapchainDesc.SampleDesc.Quality = 0;
			swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapchainDesc.BufferCount = 2;
			swapchainDesc.OutputWindow = hWnd;
			swapchainDesc.Windowed = TRUE;
			swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapchainDesc.Flags = 0;
		}

		D3D_FEATURE_LEVEL featureLevel;

		// �f�o�C�X���X���b�v�`�F�[���̐���
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			createDeviceFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&swapchainDesc,
			swapchain.GetAddressOf(),
			device.GetAddressOf(),
			&featureLevel,
			immediateContext.GetAddressOf()
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
#endif



	/// DirectX�̃o�[�W����
	D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_1;
	hr = D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, 0, createDeviceFlags,
		&featureLevels, 1, D3D11_SDK_VERSION, &device, NULL, &immediateContext);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	CreateSwapChain(dxgiFactory6.Get());
	}

// �����_�[�^�[�Q�b�g�r���[�̐���
{
	// �X���b�v�`�F�[������o�b�N�o�b�t�@�e�N�X�`�����擾����B
	// ���X���b�v�`�F�[���ɓ����Ă���o�b�N�o�b�t�@�e�N�X�`����'�F'���������ރe�N�X�`���B
	//Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	//hr = swapchain->GetBuffer(
	//	0,
	//	__uuidof(ID3D11Texture2D),
	//	reinterpret_cast<void**>(texture2d.GetAddressOf()));
	//_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	//// �o�b�N�o�b�t�@�e�N�X�`���ւ̏������݂̑����ƂȂ郌���_�[�^�[�Q�b�g�r���[�𐶐�����B
	//hr = device->CreateRenderTargetView(texture2d.Get(), nullptr, renderTargetView.GetAddressOf());
	//_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

// �[�x�X�e���V���r���[�̐���
{
	// �[�x�X�e���V�������������ނ��߂̃e�N�X�`�����쐬����B
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	D3D11_TEXTURE2D_DESC texture2dDesc;
	texture2dDesc.Width = screenWidth;
	texture2dDesc.Height = screenHeight;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2dDesc.CPUAccessFlags = 0;
	texture2dDesc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2dDesc, nullptr, texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	depthStencilViewDesc.Flags = 0;

	// �[�x�X�e���V���e�N�X�`���ւ̏������݂ɑ����ɂȂ�[�x�X�e���V���r���[���쐬����B
	//hr = device->CreateDepthStencilView(texture2d.Get(), nullptr, depthStencilView.GetAddressOf());
	hr = device->CreateDepthStencilView(texture2d.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

// �r���[�|�[�g
{
	//viewport.Width = static_cast<float>(framebufferDimensions.cx);
	//viewport.Height = static_cast<float>(framebufferDimensions.cy);
	//viewport.MinDepth = 0.0f;
	//viewport.MaxDepth = 1.0f;
	//viewport.TopLeftX = 0.0f;
	//viewport.TopLeftY = 0.0f;
	//immediateContext->RSSetViewports(1, &viewport);
}

// �����_�[�X�e�[�g����
renderState = std::make_unique<RenderState>(device.Get());

// �����_������
shapeRenderer = std::make_unique<ShapeRenderer>(device.Get());
modelRenderer = std::make_unique<ModelRenderer>(device.Get());

// �萔�o�b�t�@����
// ���Ԃ��i�[����萔�o�b�t�@
D3D11_BUFFER_DESC bufferDesc{};
bufferDesc.ByteWidth = static_cast<UINT>(sizeof(TimeCBuffer));
bufferDesc.StructureByteStride = sizeof(TimeCBuffer);
bufferDesc.Usage = D3D11_USAGE_DEFAULT;
bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
bufferDesc.MiscFlags = 0;
hr = device->CreateBuffer(&bufferDesc, nullptr, cbuffer[int(ConstantBufferType::TimeCBuffer)].ReleaseAndGetAddressOf());
_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
// ��ʃT�C�Y���i�[����萔�o�b�t�@
bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ScreenSizeCBuffer));
bufferDesc.StructureByteStride = sizeof(ScreenSizeCBuffer);
bufferDesc.Usage = D3D11_USAGE_DEFAULT;
bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
bufferDesc.MiscFlags = 0;
hr = device->CreateBuffer(&bufferDesc, nullptr, cbuffer[int(ConstantBufferType::ScreenSizeCBuffer)].ReleaseAndGetAddressOf());

_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
// �t���b�J�[���ʂ̒萔�o�b�t�@
bufferDesc.ByteWidth = static_cast<UINT>(sizeof(LightFlickerCBuffer));
bufferDesc.StructureByteStride = sizeof(LightFlickerCBuffer);
bufferDesc.Usage = D3D11_USAGE_DEFAULT;
bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
bufferDesc.MiscFlags = 0;
hr = device->CreateBuffer(&bufferDesc, nullptr, cbuffer[int(ConstantBufferType::LightFlickerCBuffer)].ReleaseAndGetAddressOf());
_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));


//framebuffer�̐���
for (int i = 0; i <int(PPShaderType::Count); i++)
{

	framebuffers[i] = std::make_unique<framebuffer>(device.Get(), screenWidth, screenHeight, true);
}

bloomer = std::make_unique<bloom>(device.Get(), 1280, 720);
bit_block_transfer = std::make_unique<fullscreen_quad>(device.Get());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/crtPS.cso", pixel_shaders[int(PPShaderType::crt)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/GlitchPS.cso", pixel_shaders[int(PPShaderType::Glitch)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/SharpenPS.cso", pixel_shaders[int(PPShaderType::Sharpen)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/BreathShakePS.cso", pixel_shaders[int(PPShaderType::BreathShake)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/TemporalNoisePS.cso", pixel_shaders[int(PPShaderType::TemporalNoise)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/NoSignalFinalePS.cso", pixel_shaders[int(PPShaderType::NoSignalFinale)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/VisionBootDownPS.cso", pixel_shaders[int(PPShaderType::VisionBootDown)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/CrackshaftPS.cso", pixel_shaders[int(PPShaderType::Crackshaft)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/HighLightPassPS.cso", pixel_shaders[int(PPShaderType::HighLightPass)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/BlurPS.cso", pixel_shaders[int(PPShaderType::Blur)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/BloomFinal.cso", pixel_shaders[int(PPShaderType::BloomFinal)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/TVNoiseFadePS.cso", pixel_shaders[int(PPShaderType::TVNoiseFade)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/GameOverPS.cso", pixel_shaders[int(PPShaderType::GameOver)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/FilmGrainDustPS.cso", pixel_shaders[int(PPShaderType::FilmGrainDust)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/FadeToBlackPS.cso", pixel_shaders[int(PPShaderType::FadeToBlack)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/WardenGazePS.cso", pixel_shaders[int(PPShaderType::WardenGaze)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/NoiseChangePS.cso", pixel_shaders[int(PPShaderType::NoiseChange)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/LightFlickerPS.cso", pixel_shaders[int(PPShaderType::LightFlicker)].ReleaseAndGetAddressOf());
GpuResourceUtils::LoadPixelShader(device.Get(), "Data/Shader/TimerPS.cso", pixel_shaders[int(PPShaderType::Timer)].ReleaseAndGetAddressOf());



}

// �N���A
void Graphics::Clear(float r, float g, float b, float a)
{
	float color[4]{ r, g, b, a };
	immediateContext->ClearRenderTargetView(renderTargetView.Get(), color);
	immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

// �����_�[�^�[�Q�b�g�ݒ�
void Graphics::SetRenderTargets()
{
	//immediateContext->RSSetViewports(1, &viewport);
	immediateContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
}

// ��ʕ\��
void Graphics::Present(UINT syncInterval)
{
	swapchain->Present(syncInterval, 0);
}

void Graphics::OnResize(UINT64 width, UINT height)
{
	// �t���[���o�b�t�@�̃T�C�Y�̍X�V
	if (width > 0 && height > 0 && (width != framebufferDimensions.cx || height != framebufferDimensions.cy))
	{
		framebufferDimensions.cx = static_cast<LONG>(width);
		framebufferDimensions.cy = height;

		// �X�V���ꂽ���@�Ńt���[���o�b�t�@���č쐬
		if (device)
		{
			for (int i = 0; i <int(PPShaderType::Count); i++)
			{
				framebuffers[i] = std::make_unique<framebuffer>(device.Get(), framebufferDimensions.cx, framebufferDimensions.cy, true);
			}

			// �T�C�Y�ύX����O�ɁA���ׂĂ̈ȑO��GPU�R�}���h���I�����Ă��邱�Ƃ��m�F
			immediateContext->Flush();
			immediateContext->ClearState();

			// �X���b�v�`�F�[���č쐬
			CreateSwapChain(dxgiFactory6.Get());

			Input::Instance().GetMouse().SetScreenWidth(framebufferDimensions.cx);
			Input::Instance().GetMouse().SetScreenHeight(framebufferDimensions.cy);
		}
	}
}

void Graphics::StylizeWindow(BOOL screenMode)
{
	this->screenMode = screenMode;

	// �t���X�N���[�����[�h
	if (screenMode)
	{
		// �E�B���h�E���[�h�ŃE�B���h�E�̈ʒu���L�^����
		GetWindowRect(hWnd, &windowedRect);

		// �S��ʗp�ɃE�B���h�E�X�^�C����ύX����i�^�C�g���o�[�ƃ{�[�_�[���폜����j
		DWORD fullscreenStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME);
		SetWindowLongPtrA(hWnd, GWL_STYLE, fullscreenStyle);

		RECT fullscreenWindowRect;

		HRESULT result = E_FAIL;
		if (swapchain)
		{
			Microsoft::WRL::ComPtr<IDXGIOutput> dxgiOutput;
			result = swapchain->GetContainingOutput(&dxgiOutput); // ���݂̃X���b�v�`�F�[���̏o�̓f�o�C�X���擾
			if (result == S_OK)
			{
				DXGI_OUTPUT_DESC outputDesc;
				result = dxgiOutput->GetDesc(&outputDesc); // �o�̓f�o�C�X�̉𑜓x���擾����
				if (result == S_OK)
				{
					fullscreenWindowRect = outputDesc.DesktopCoordinates; // �t���X�N���[���E�B���h�E�̍��W��ݒ肷��
				}
			}
		}

		if (result != S_OK)
		{
			// �X���b�v�`�F�[������̉𑜓x�擾�Ɏ��s�����ꍇ�A�f�B�X�v���C�ݒ���擾���܂��B
			DEVMODE devmode = {};
			devmode.dmSize = sizeof(DEVMODE);
			EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);

			// �擾�����\���𑜓x���g�p���ăt���X�N���[���E�B���h�E�̃T�C�Y�����肵�Ă�������
			fullscreenWindowRect = {
				devmode.dmPosition.x,
				devmode.dmPosition.y,
				devmode.dmPosition.x + static_cast<LONG>(devmode.dmPelsWidth),
				devmode.dmPosition.y + static_cast<LONG>(devmode.dmPelsHeight)
			};
		}
		// �t���X�N���[���p�ɃE�B���h�E�̈ʒu�ƃT�C�Y��ݒ肷��
		SetWindowPos(
			hWnd,
			NULL,
			fullscreenWindowRect.left,
			fullscreenWindowRect.top,
			fullscreenWindowRect.right,
			fullscreenWindowRect.bottom,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);

		ShowWindow(hWnd, SW_MAXIMIZE);
	}
	// �ʏ�E�B���h�E���[�h
	else
	{
		// �E�B���h�E���[�h�ݒ�
		DEVMODE devmode = {};
		devmode.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);

		// �E�B���h�E���[�h�X�^�C����ݒ肷��i�ő剻�{�^���ƃT�C�Y�ύX�𖳌��ɂ���j
		DWORD windowedStyle = WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE;
		SetWindowLongPtrA(hWnd, GWL_STYLE, windowedStyle);
		SetWindowPos(
			hWnd,
			HWND_NOTOPMOST,
			windowedRect.left,
			windowedRect.top,
			windowedRect.right - windowedRect.left,
			windowedRect.bottom - windowedRect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);

		ShowWindow(hWnd, SW_NORMAL); // �ʏ�̃E�B���h�E�Ƃ��ĕ\������
	}
}

DirectX::XMFLOAT2 Graphics::GetWindowScaleFactor() const
{
	DirectX::XMFLOAT2 windowScaleFactor = { framebufferDimensions.cx / screenWidth, framebufferDimensions.cy / screenHeight };
	return windowScaleFactor;
}

void Graphics::AcquireHighPerformanceAdapter(IDXGIFactory6* dxgiFactory6, IDXGIAdapter3** dxgiAdapter3)
{
	HRESULT result = S_OK;

	Microsoft::WRL::ComPtr<IDXGIAdapter3> enumerated_adapter;
	for (UINT adapter_index = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory6->EnumAdapterByGpuPreference(adapter_index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(enumerated_adapter.ReleaseAndGetAddressOf())); ++adapter_index)
	{
		DXGI_ADAPTER_DESC1 adapter_desc;
		result = enumerated_adapter->GetDesc1(&adapter_desc);
		_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

		if (adapter_desc.VendorId == 0x1002/*AMD*/ || adapter_desc.VendorId == 0x10DE/*NVIDIA*/)
		{
			OutputDebugStringW((std::wstring(adapter_desc.Description) + L" has been selected.\n").c_str());
			OutputDebugStringA(std::string("\tVendorId:" + std::to_string(adapter_desc.VendorId) + '\n').c_str());
			OutputDebugStringA(std::string("\tDeviceId:" + std::to_string(adapter_desc.DeviceId) + '\n').c_str());
			OutputDebugStringA(std::string("\tSubSysId:" + std::to_string(adapter_desc.SubSysId) + '\n').c_str());
			OutputDebugStringA(std::string("\tRevision:" + std::to_string(adapter_desc.Revision) + '\n').c_str());
			OutputDebugStringA(std::string("\tDedicatedVideoMemory:" + std::to_string(adapter_desc.DedicatedVideoMemory) + '\n').c_str());
			OutputDebugStringA(std::string("\tDedicatedSystemMemory:" + std::to_string(adapter_desc.DedicatedSystemMemory) + '\n').c_str());
			OutputDebugStringA(std::string("\tSharedSystemMemory:" + std::to_string(adapter_desc.SharedSystemMemory) + '\n').c_str());
			OutputDebugStringA(std::string("\tAdapterLuid.HighPart:" + std::to_string(adapter_desc.AdapterLuid.HighPart) + '\n').c_str());
			OutputDebugStringA(std::string("\tAdapterLuid.LowPart:" + std::to_string(adapter_desc.AdapterLuid.LowPart) + '\n').c_str());
			OutputDebugStringA(std::string("\tFlags:" + std::to_string(adapter_desc.Flags) + '\n').c_str());
			break;
		}
	}
	*dxgiAdapter3 = enumerated_adapter.Detach();
}

void Graphics::CreateSwapChain(IDXGIFactory6* dxgiFactory6)
{
	HRESULT hr = S_OK;

	if (swapchain)
	{
		renderTargetView.Reset();
		depthStencilView.Reset();

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapchain->GetDesc(&swapChainDesc);
		hr = swapchain->ResizeBuffers(swapChainDesc.BufferCount, framebufferDimensions.cx, framebufferDimensions.cy, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBuffer;
		hr = swapchain->GetBuffer(0, IID_PPV_ARGS(renderTargetBuffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		D3D11_TEXTURE2D_DESC texture2dDesc;
		renderTargetBuffer->GetDesc(&texture2dDesc);

		hr = device->CreateRenderTargetView(renderTargetBuffer.Get(), NULL, renderTargetView.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// �[�x�X�e���V�������������ނ��߂̃e�N�X�`�����쐬����B
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		//D3D11_TEXTURE2D_DESC texture2dDesc;
		texture2dDesc.Width = framebufferDimensions.cx;
		texture2dDesc.Height = framebufferDimensions.cy;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2dDesc, nullptr, texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		depthStencilViewDesc.Flags = 0;

		// �[�x�X�e���V���e�N�X�`���ւ̏������݂ɑ����ɂȂ�[�x�X�e���V���r���[���쐬����B
		//hr = device->CreateDepthStencilView(texture2d.Get(), nullptr, depthStencilView.GetAddressOf());
		hr = device->CreateDepthStencilView(texture2d.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	else
	{
		BOOL allowTearing = FALSE;
		hr = dxgiFactory6->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
		tearingSupported = SUCCEEDED(hr) && allowTearing;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc1{};
		swapChainDesc1.Width = framebufferDimensions.cx;
		swapChainDesc1.Height = framebufferDimensions.cy;
		swapChainDesc1.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc1.Stereo = FALSE;
		swapChainDesc1.SampleDesc.Count = 1;
		swapChainDesc1.SampleDesc.Quality = 0;
		swapChainDesc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc1.BufferCount = 2;
		swapChainDesc1.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc1.Flags = tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
		hr = dxgiFactory6->CreateSwapChainForHwnd(device.Get(), hWnd, &swapChainDesc1, NULL, NULL, swapchain.ReleaseAndGetAddressOf());
#if 0
		swap_chain_desc1.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
#endif
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = dxgiFactory6->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
		hr = swapchain->GetBuffer(0, IID_PPV_ARGS(render_target_buffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		hr = device->CreateRenderTargetView(render_target_buffer.Get(), NULL, renderTargetView.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(framebufferDimensions.cx);
	viewport.Height = static_cast<float>(framebufferDimensions.cy);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	immediateContext->RSSetViewports(1, &viewport);
}
