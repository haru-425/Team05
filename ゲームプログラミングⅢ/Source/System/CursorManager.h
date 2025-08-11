#pragma once

#include "RenderContext.h"
#include "Sprite.h"
#include "Input.h"

#define CURSOR_VISION 1 // カーソル表示切り替え用（未使用の定義）

//=============================================================
// CursorManager
// マウスカーソルの描画・表示制御を担当するシングルトン
//=============================================================
class CursorManager
{
public:
	//=============================================================
	// インスタンス取得（シングルトン）
	// 呼び出すたびに同じインスタンスが返される
	//=============================================================
	static CursorManager& Instance() {
		static CursorManager instance;
		return instance;
	}

	//=============================================================
	// 更新関数（現状は空。将来的にカーソルアニメや状態管理を追加可能）
	//=============================================================
	void Update(float elapsedTime)
	{
		// ※未実装
	}

	//=============================================================
	// カーソルの描画処理
	// カーソルが表示ONかつウィンドウがアクティブなときのみ描画する
	//=============================================================
	void Render()
	{
		ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
		Graphics& graphics = Graphics::Instance();
		RenderState* renderState = graphics.GetRenderState();

		// 描画準備用コンテキストを設定
		RenderContext rc;
		rc.deviceContext = dc;
		rc.renderState = graphics.GetRenderState();

		// カーソルが表示中かつウィンドウがアクティブな場合に描画
		if (isVisibleCursor && isActiveWindow)
		{
			// カーソル画像を現在のマウス座標位置に描画
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
			//	0,     // Z値（深度）
			//	25, 40,// 描画サイズ（幅・高さ）
			//	0, 0,  // UV座標（始点）
			//	558, 846, // UVサイズ（元画像からの切り出しサイズ）
			//	0,      // 回転角
			//	1, 1, 1, 1 // 色（RGBA）＝不透明白
			//);
		}
	}

	//=============================================================
	// アクティブウィンドウの状態を設定
	// 非アクティブ時はカーソルを表示、アクティブ時はカーソルを非表示にする（リリースビルドのみ）
	//=============================================================
	void SetActiveWindow(bool isActive) {
		isActiveWindow = isActive;

#if _DEBUG
		// デバッグ中はShowCursorを変更しない
#else
		// ウィンドウが非アクティブな場合 → カーソルを表示するように強制
		if (!isActiveWindow && ShowCursor(true) < 1)
		{
			ShowCursor(true);
			while (ShowCursor(true) < 0)
			{
				ShowCursor(true);
			}
		}
		// ウィンドウがアクティブな場合 → カーソルを非表示にする
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
	// ウィンドウがアクティブかどうかを取得
	//=============================================================
	bool GetIsActiveWindow() { return isActiveWindow; }

	//=============================================================
	// カーソルの表示・非表示を設定
	//=============================================================
	void SetCursorVisible(bool isVisible) { isVisibleCursor = isVisible; }

	//=============================================================
	// カーソル用のスプライトを読み込み
	//=============================================================
	void SetCursor() {
		Cursor = std::make_unique<Sprite>("Data/Sprite/dummy_cursor.png");
	}

private:
	//=============================================================
	// コンストラクタ（外部から生成できない）
	//=============================================================
	CursorManager() {}

	//=============================================================
	// デストラクタ
	//=============================================================
	~CursorManager() {}

	//=============================================================
	// メンバ変数
	//=============================================================
	bool isActiveWindow;             // ウィンドウがアクティブかどうか
	bool isVisibleCursor = true;    // カーソルが表示状態かどうか
	std::unique_ptr<Sprite> Cursor; // 描画用カーソル画像
};
