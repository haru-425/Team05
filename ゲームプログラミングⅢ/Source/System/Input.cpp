#include "System/Input.h"

// 初期化
void Input::Initialize(HWND hWnd)
{
	gamePad = std::make_unique<GamePad>();
	mouse = std::make_unique<Mouse>(hWnd);
}

// 更新処理
void Input::Update()
{
	gamePad->Update();
	mouse->Update();

	/// どのデバイスがアクティブか
	bool activeGamePad = false , activeMouse = false;
	activeGamePad	= gamePad->GetIsActive();
	activeMouse		= mouse->GetIsActive();
	if (activeGamePad)
	{
		isActiveGamePad = true;
		isActiveMouse = false;
	}
	if (activeMouse)
	{
		isActiveMouse = true;
		isActiveGamePad = false;
	}
}
