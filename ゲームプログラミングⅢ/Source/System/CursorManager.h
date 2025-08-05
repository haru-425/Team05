#pragma once
#include "RenderContext.h"
#include "Sprite.h"
#include "Input.h"

#define CURSOR_VISION 1

class CursorManager
{
public:
	// インスタンス取得
	static CursorManager& Instance() {
		static CursorManager instance;
		return instance;
	}

	void Update(float elapsedTime)
	{

	}

	void Render()
	{
		ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
		Graphics& graphics = Graphics::Instance();
		RenderState* renderState = graphics.GetRenderState();

		//描画準備
		RenderContext rc;
		rc.deviceContext = dc;
		rc.renderState = graphics.GetRenderState();

		if (isVisibleCursor && isActiveWindow)
		{
			Cursor->Render(rc, Input::Instance().GetMouse().GetPositionX() / Graphics::Instance().GetWindowScaleFactor().x, Input::Instance().GetMouse().GetPositionY() / Graphics::Instance().GetWindowScaleFactor().y, 0, 25, 40, 0, 0, 558, 846, 0, 1, 1, 1, 1);
		}

	}

	void SetActiveWindow(bool isActive) {
		isActiveWindow = isActive; 


#if _DEBUG

#else
		if (!isActiveWindow && ShowCursor(true) < 1)
		{
			ShowCursor(true);
			while (ShowCursor(true) < 0)
			{

				ShowCursor(true);
			}
		}
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

	bool GetIsActiveWindow() {return isActiveWindow;}

	void SetCursorVisible(bool isVisible) { isVisibleCursor = isVisible; }

	void SetCursor() { Cursor = std::make_unique<Sprite>("Data/Sprite/dummy_cursor.png"); }

private:
	CursorManager() {}
	~CursorManager() {}

	bool isActiveWindow;
	bool isVisibleCursor=true;
	std::unique_ptr<Sprite> Cursor;

};