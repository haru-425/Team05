#pragma once
#include "RenderContext.h"
#include "Sprite.h"

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

	void Render(const RenderContext& rc)
	{
#if 0


#endif // 1

	}

	void SetActiveWindow(bool isActive) {
		isActiveWindow = isActive; 
		if (!isActiveWindow && ShowCursor(true) < 1)
		{
			ShowCursor(true);
			while (ShowCursor(true) < 0)
			{
				
				int a =ShowCursor(true);
			}
		}
#if 1
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

private:
	CursorManager() {}
	~CursorManager() {}

	bool isActiveWindow;
	bool isVisibleCursor;

};