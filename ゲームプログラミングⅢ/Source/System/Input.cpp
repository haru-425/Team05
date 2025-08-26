#include "System/Input.h"

// ������
void Input::Initialize(HWND hWnd)
{
	gamePad = std::make_unique<GamePad>();
	mouse = std::make_unique<Mouse>(hWnd);
}

// �X�V����
void Input::Update()
{
	gamePad->Update();
	mouse->Update();

	/// �ǂ̃f�o�C�X���A�N�e�B�u��
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
