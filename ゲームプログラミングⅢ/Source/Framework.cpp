#include <memory>
#include <sstream>
#include <imgui.h>
#include <Windows.h>

#include "Framework.h"
#include "System/Input.h"
#include "System/Graphics.h"
#include "System/ImGuiRenderer.h"
#include "SceneGame.h"
#include "SceneTitle.h"
#include "SceneMattsu.h"
#include"SceneGraphics.h"
#include "SceneLogo.h"
#include "SceneManager.h"
#include "System/Audio.h"
#include "System/SettingsManager.h"
#include "stdio.h"
#include "System/ResourceManager.h"
#include "System/Input.h"
#include "System/CursorManager.h"

// 垂直同期間隔設定
static const int syncInterval = 1;

// コンストラクタ
Framework::Framework(HWND hWnd)
	: hWnd(hWnd)
{
	// 設定読み込み
	SettingsManager::Instance().Load();

	// オーディオ初期化
	Audio::Instance().Initialize();

	hDC = GetDC(hWnd);

	// インプット初期化
	Input::Instance().Initialize(hWnd);

	// グラフィックス初期化
	Graphics::Instance().Initialize(hWnd);

	// IMGUI初期化
	ImGuiRenderer::Initialize(hWnd, Graphics::Instance().GetDevice(), Graphics::Instance().GetDeviceContext());

	// シーン初期化
	//SceneManager::instance().ChangeScene(new SceneTitle);
	SceneManager::instance().ChangeScene(new SceneLogo(new SceneTitle));
}

// デストラクタ
Framework::~Framework()
{
	// シーン終了化
	//sceneGame.Finalize();
	SceneManager::instance().Clear();

	// IMGUI終了化
	ImGuiRenderer::Finalize();

	ReleaseDC(hWnd, hDC);

	ResourceManager::Instance().Clear();

	Audio::Instance().Finalize();
}

// 更新処理
void Framework::Update(float elapsedTime)
{
	static int count = 0;
#ifndef _DEBUG
	if (count == 1)
		Graphics::Instance().StylizeWindow(Graphics::Instance().GetScreenMode());
#endif

	// インプット更新処理
	Input::Instance().Update();

	// IMGUIフレーム開始処理
	ImGuiRenderer::NewFrame();

	// シーン更新処理
	//sceneGame.Update(elapsedTime);
	SceneManager::instance().Update(elapsedTime);

#ifdef _DEBUG
	GamePad& input = Input::Instance().GetGamePad();
	if ((input.GetButtonDown() & GamePad::F2))
	{
		Graphics::Instance().StylizeWindow(!Graphics::Instance().GetScreenMode());
	}
#endif

	count++;
	CursorManager::Instance().SetActiveWindow(GetForegroundWindow() == hWnd);

}
float elapsed = 0;
// 描画処理
void Framework::Render(float elapsedTime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	// 画面クリア
	Graphics::Instance().Clear(0, 0, 0, 1);

	// レンダーターゲット設定
	Graphics::Instance().SetRenderTargets();

	// シーン描画処理
	SceneManager::instance().Render();

	// シーンGUI描画処理
#if defined _DEBUG
	SceneManager::instance().DrawGUI();
	if (Graphics::Instance().GetScreenMode())
	{
		if (ImGui::Begin("FPS"))
		{
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		ImGui::End();
	}
	if (ImGui::Begin("elapsedTime"))
	{
		char buffer[256];
		//sprintf_s(&buffer, "elapsedTime :%f", elapsed);
		ImGui::Text("elapsedTime : %f", elapsed);
	}
	ImGui::End();
#endif

#if 0
	// IMGUIデモウインドウ描画（IMGUI機能テスト用）
	ImGui::ShowDemoWindow();
#endif
	// IMGUI描画
	ImGuiRenderer::Render(dc);

	// 画面表示
	Graphics::Instance().Present(syncInterval);
}

// フレームレート計算
void Framework::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the
	// average time it takes to render one frame.  These stats
	// are appended to the window caption bar.
	static int frames = 0;
	static float time_tlapsed = 0.0f;

	frames++;

	// Compute averages over one second period.
	if ((timer.TimeStamp() - time_tlapsed) >= 1.0f)
	{
		float fps = static_cast<float>(frames); // fps = frameCnt / 1
		float mspf = 1000.0f / fps;
		std::ostringstream outs;
		outs.precision(6);
		outs << "Rubot" /*FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)*/;
		SetWindowTextA(hWnd, outs.str().c_str());

		// Reset for next average.
		frames = 0;
		time_tlapsed += 1.0f;
	}
}

// アプリケーションループ
int Framework::Run()
{
	MSG msg = {};

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.Tick();
			CalculateFrameStats();

			//float elapsedTime = syncInterval == 0
			//	? timer.TimeInterval()
			//	: syncInterval / static_cast<float>(GetDeviceCaps(hDC, VREFRESH))
			//	;
			float elapsedTime = timer.TimeInterval();
			if (elapsedTime >= 1.0f / 60.0f)
			{
				elapsedTime = 1.0 / 60.0f;
			}
			Update(elapsedTime);

			Render(elapsedTime);
			elapsed = elapsedTime;

			/// 終了処理
			if (SceneManager::instance().GetIsExit())
				break;
		}
	}
	return static_cast<int>(msg.wParam);
}

// メッセージハンドラ
LRESULT CALLBACK Framework::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGuiRenderer::HandleMessage(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
#ifdef _DEBUG
		if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
#endif
		break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		timer.Stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		timer.Start();
		break;

	case WM_SIZE:
	{
		RECT clientRect = {};
		GetClientRect(hWnd, &clientRect);
		Graphics::Instance().OnResize(static_cast<UINT64>(clientRect.right - clientRect.left), clientRect.bottom - clientRect.top);
	}
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
