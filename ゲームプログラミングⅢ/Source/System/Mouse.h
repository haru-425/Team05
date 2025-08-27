#pragma once

#include <Windows.h>
#include <algorithm>
#include <DirectXMath.h>

using MouseButton = unsigned int;

// �}�E�X
class Mouse
{
public:
	static const MouseButton BTN_LEFT = (1 << 0);
	static const MouseButton BTN_MIDDLE = (1 << 1);
	static const MouseButton BTN_RIGHT = (1 << 2);

public:
	Mouse(HWND hWnd);
	~Mouse() {}

	// �X�V
	void Update();

	// �{�^�����͏�Ԃ̎擾
	MouseButton GetButton() const { return buttonState[0]; }

	// �{�^��������Ԃ̎擾
	MouseButton GetButtonDown() const { return buttonDown; }

	// �{�^�������Ԃ̎擾
	MouseButton GetButtonUp() const { return buttonUp; }

	// �z�C�[���l�̐ݒ�
	void SetWheel(int wheel) { this->wheel[0] += wheel; }

	// �z�C�[���l�̎擾
	int GetWheel() const { return wheel[1]; }

	// �}�E�X�J�[�\��X���W�擾
	int GetPositionX() const { return positionX[0]; }

	// �}�E�X�J�[�\��Y���W�擾
	int GetPositionY() const { return positionY[0]; }

	// �O��̃}�E�X�J�[�\��X���W�擾
	int GetOldPositionX() const { return positionX[1]; }

	// �O��̃}�E�X�J�[�\��Y���W�擾
	int GetOldPositionY() const { return positionY[1]; }

	// �X�N���[�����ݒ�
	void SetScreenWidth(int width) { screenWidth = width; }

	// �X�N���[�������ݒ�
	void SetScreenHeight(int height) { screenHeight = height; }

	// �X�N���[�����擾
	int GetScreenWidth() const { return screenWidth; }

	// �X�N���[�������擾
	int GetScreenHeight() const { return screenHeight; }

	/**
	* @brief �}�E�X���g���Ă��邩
	* 
	* �}�E�X�͏�ɔ������邪�A�}�E�X�����ۂɎg���Ă���̂��ǂ�����
	* ���ׂ�֐��ł�
	* 
	* @return true : �g�p�� | false : ���g�p
	*/
	bool GetIsActive()
	{
		bool useMouse;
		DirectX::XMINT2 currMousePos = { positionX[0], positionY[0] };
		DirectX::XMINT2 prevMousePos = { positionX[1], positionY[1] };
		DirectX::XMINT2 mouseDelta = { currMousePos.x - prevMousePos.x , currMousePos.y - prevMousePos.y };
		useMouse = std::abs(mouseDelta.x) || std::abs(mouseDelta.y);

		return useMouse;
	}

private:
	MouseButton		buttonState[2] = { 0 };
	MouseButton		buttonDown = 0;
	MouseButton		buttonUp = 0;
	int				positionX[2];
	int				positionY[2];
	int				wheel[2];
	int				screenWidth = 0;
	int				screenHeight = 0;
	HWND			hWnd = nullptr;
};
