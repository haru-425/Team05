#pragma once

#include "RenderContext.h"
#include "Sprite.h"
#include "Input.h"

#define CURSOR_VISION 1 // �J�[�\���\���؂�ւ��p�i���g�p�̒�`�j

//=============================================================
// CursorManager
// �}�E�X�J�[�\���̕`��E�\�������S������V���O���g��
//=============================================================
class CursorManager
{
public:
	//=============================================================
	// �C���X�^���X�擾�i�V���O���g���j
	// �Ăяo�����тɓ����C���X�^���X���Ԃ����
	//=============================================================
	static CursorManager& Instance() {
		static CursorManager instance;
		return instance;
	}

	//=============================================================
	// �X�V�֐��i����͋�B�����I�ɃJ�[�\���A�j�����ԊǗ���ǉ��\�j
	//=============================================================
	void Update(float elapsedTime)
	{
		// ��������
	}

	//=============================================================
	// �J�[�\���̕`�揈��
	// �J�[�\�����\��ON���E�B���h�E���A�N�e�B�u�ȂƂ��̂ݕ`�悷��
	//=============================================================
	void Render()
	{
		ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
		Graphics& graphics = Graphics::Instance();
		RenderState* renderState = graphics.GetRenderState();

		// �`�揀���p�R���e�L�X�g��ݒ�
		RenderContext rc;
		rc.deviceContext = dc;
		rc.renderState = graphics.GetRenderState();

		// �J�[�\�����\�������E�B���h�E���A�N�e�B�u�ȏꍇ�ɕ`��
		if (isVisibleCursor && isActiveWindow)
		{
			// �J�[�\���摜�����݂̃}�E�X���W�ʒu�ɕ`��
			Cursor->Render(
				rc,
				(Input::Instance().GetMouse().GetPositionX() / Graphics::Instance().GetWindowScaleFactor().x - (40 / 2 /*- i * 20*/) /** scaleX */ / 2.f),
				(Input::Instance().GetMouse().GetPositionY() / Graphics::Instance().GetWindowScaleFactor().y - (40 / 2 /*- i * 20*/) /** scaleY*/ / 2.f),
				0.0f,
				(40 /*- i * 20*/) / 2 /** scaleX*/,
				(40 /*- i * 20*/) / 2/* * scaleY*/,
				0,
				1.0f, 1.0f, 1.0f, 1.0f
			);
			//Cursor->Render(
			//	rc,
			//	Input::Instance().GetMouse().GetPositionX() / Graphics::Instance().GetWindowScaleFactor().x,
			//	Input::Instance().GetMouse().GetPositionY() / Graphics::Instance().GetWindowScaleFactor().y,
			//	0,     // Z�l�i�[�x�j
			//	25, 40,// �`��T�C�Y�i���E�����j
			//	0, 0,  // UV���W�i�n�_�j
			//	558, 846, // UV�T�C�Y�i���摜����̐؂�o���T�C�Y�j
			//	0,      // ��]�p
			//	1, 1, 1, 1 // �F�iRGBA�j���s������
			//);
		}
	}

	//=============================================================
	// �A�N�e�B�u�E�B���h�E�̏�Ԃ�ݒ�
	// ��A�N�e�B�u���̓J�[�\����\���A�A�N�e�B�u���̓J�[�\�����\���ɂ���i�����[�X�r���h�̂݁j
	//=============================================================
	void SetActiveWindow(bool isActive) {
		isActiveWindow = isActive;

#if _DEBUG
		// �f�o�b�O����ShowCursor��ύX���Ȃ�
#else
		// �E�B���h�E����A�N�e�B�u�ȏꍇ �� �J�[�\����\������悤�ɋ���
		if (!isActiveWindow && ShowCursor(true) < 1)
		{
			ShowCursor(true);
			while (ShowCursor(true) < 0)
			{
				ShowCursor(true);
			}
		}
		// �E�B���h�E���A�N�e�B�u�ȏꍇ �� �J�[�\�����\���ɂ���
		else if (isActiveWindow && ShowCursor(false) > -1)
		{
			ShowCursor(false);
			while (ShowCursor(false) > 0)
			{
				ShowCursor(false);
			}
		}
#endif
	}

	//=============================================================
	// �E�B���h�E���A�N�e�B�u���ǂ������擾
	//=============================================================
	bool GetIsActiveWindow() { return isActiveWindow; }

	//=============================================================
	// �J�[�\���̕\���E��\����ݒ�
	//=============================================================
	void SetCursorVisible(bool isVisible) { isVisibleCursor = isVisible; }

	//=============================================================
	// �J�[�\���p�̃X�v���C�g��ǂݍ���
	//=============================================================
	void SetCursor() {
		Cursor = std::make_unique<Sprite>("Data/Sprite/dummy_cursor.png");
	}

private:
	//=============================================================
	// �R���X�g���N�^�i�O�����琶���ł��Ȃ��j
	//=============================================================
	CursorManager() {}

	//=============================================================
	// �f�X�g���N�^
	//=============================================================
	~CursorManager() {}

	//=============================================================
	// �����o�ϐ�
	//=============================================================
	bool isActiveWindow;             // �E�B���h�E���A�N�e�B�u���ǂ���
	bool isVisibleCursor = true;    // �J�[�\�����\����Ԃ��ǂ���
	std::unique_ptr<Sprite> Cursor; // �`��p�J�[�\���摜
};
