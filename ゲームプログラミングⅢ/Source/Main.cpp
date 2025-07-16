#include <windows.h>
#include <memory>
#include <assert.h>
#include <tchar.h>

#include "Framework.h"

const LONG SCREEN_WIDTH = 1280;
const LONG SCREEN_HEIGHT = 720;

LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Framework* f = reinterpret_cast<Framework*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	return f ? f->HandleMessage(hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
}

INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, INT cmd_show)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(237);
#endif
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = fnWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = 0;

	LoadIcon(wcex.hInstance, MAKEINTRESOURCE(333));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("Game");
	wcex.hIconSm = 0;
	RegisterClassEx(&wcex);

	RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	HWND hWnd = CreateWindow(_T("Game"), _T(""), WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, instance, NULL);
	ShowWindow(hWnd, cmd_show);

	Framework f(hWnd);
	SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&f));

	// アイコンをリソースから読み込む
	HICON hIcon = (HICON)LoadImage(
		GetModuleHandle(NULL),
		MAKEINTRESOURCE(3333),  // .rcで定義したID
		IMAGE_ICON,
		32, 32,                      // サイズ（タスクバー向け）
		LR_DEFAULTCOLOR
	);

	HICON hIconSmall = (HICON)LoadImage(
		GetModuleHandle(NULL),
		MAKEINTRESOURCE(333),
		IMAGE_ICON,
		16, 16,                      // 小アイコン（ウィンドウ左上）
		LR_DEFAULTCOLOR
	);

	// ウィンドウにアイコンを設定
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);       // タスクバー用
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
	return f.Run();
}
