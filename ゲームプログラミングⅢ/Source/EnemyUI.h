#pragma once
#include <DirectXMath.h>
#include "System/Sprite.h"
#include "System/RenderContext.h"
#include "Camera/Camera.h"
#include "System/Graphics.h"

using namespace DirectX;

class EnemyUI {
public:
	static EnemyUI& Instance() {
		static EnemyUI instance;
		return instance;
	}
	inline XMFLOAT2 WorldToScreen(
		const XMFLOAT3 worldPos,
		const XMFLOAT4X4& viewMatrix,
		const XMFLOAT4X4& projMatrix,
		float viewportWidth,
		float viewportHeight)
	{
		using namespace DirectX;

		XMMATRIX view = XMLoadFloat4x4(&viewMatrix);
		XMMATRIX proj = XMLoadFloat4x4(&projMatrix);

		XMFLOAT3 adjustedPos = worldPos;
		adjustedPos.y += 1.0f;  // 高さ調整（必要に応じて）
		XMVECTOR pos = XMLoadFloat3(&adjustedPos);

		XMMATRIX vp = XMMatrixMultiply(view, proj);
		XMVECTOR projected = XMVector3TransformCoord(pos, vp);

		// 画面の中央
		XMFLOAT2 center(viewportWidth * 0.5f, viewportHeight * 0.5f);

		// 視野外なら画面中央を返す
		if (projected.m128_f32[2] < 0.0f || projected.m128_f32[2] > 1.0f ||
			projected.m128_f32[0] < -1.0f || projected.m128_f32[0] > 1.0f ||
			projected.m128_f32[1] < -1.0f || projected.m128_f32[1] > 1.0f)
		{
			return center;
		}

		// NDC → スクリーン座標へ
		float x = (projected.m128_f32[0] + 1.0f) * 0.5f * viewportWidth;
		float y = (1.0f - projected.m128_f32[1]) * 0.5f * viewportHeight;

		return XMFLOAT2(x, y);
	}

	void Initialize() {
		ui[0] = new Sprite("Data/Sprite/crosshair1.png");
		ui[1] = new Sprite("Data/Sprite/crosshair2.png");
		ui[2] = new Sprite("Data/Sprite/crosshair3.png");

		for (int i = 0; i < 3; ++i) {
			uiPos[i] = XMFLOAT2(640.0f, 360.0f); // 初期位置：画面中央
		}
	}

	void Finalize() {
		for (int i = 0; i < 3; ++i) {
			delete ui[i];
		}
	}

	void Update(float elapsedTime, const XMFLOAT3& playerWorldPos, bool playerDetected) {
		XMFLOAT2 targetPos;
		if (playerDetected)
		{
			targetPos = WorldToScreen(
				playerWorldPos,
				Camera::Instance().GetView(),
				Camera::Instance().GetProjection(),
				Graphics::Instance().GetScreenWidth(),
				Graphics::Instance().GetScreenHeight()
			);
		}
		else
		{
			targetPos = XMFLOAT2(
				Graphics::Instance().GetScreenWidth() / 2.f,
				Graphics::Instance().GetScreenHeight() / 2.f
			);
		}

		// 先頭クロスヘア：プレイヤーに直接追従
		{
			float speed = std::clamp(6.0f * elapsedTime, 0.0f, 1.0f);
			XMVECTOR current = XMLoadFloat2(&uiPos[0]);
			XMVECTOR target = XMLoadFloat2(&targetPos);
			XMVECTOR result = XMVectorLerp(current, target, speed);
			XMStoreFloat2(&uiPos[0], result);
		}

		// 後続クロスヘア：ひとつ前に追従
		for (int i = 1; i < 3; ++i) {
			float rawSpeed = 20.0f - i * 2.0f;  // 減衰速度
			float followSpeed = std::clamp(rawSpeed * elapsedTime, 0.0f, 1.0f);

			XMVECTOR current = XMLoadFloat2(&uiPos[i]);
			XMVECTOR target = XMLoadFloat2(&uiPos[i - 1]);
			XMVECTOR result = XMVectorLerp(current, target, followSpeed);
			XMStoreFloat2(&uiPos[i], result);
		}
	}


	void Render(const RenderContext& rc) {
		for (int i = 0; i < 3; ++i) {
			ui[i]->Render(
				rc,
				uiPos[i].x - (160 - i * 20) / 2.f,
				uiPos[i].y - (160 - i * 20) / 2.f,
				0.0f,
				160 - i * 20,
				160 - i * 20,
				0,
				1.0f, 1.0f, 1.0f, 1.0f
			);
		}
	}

private:
	Sprite* ui[3];         // クロスヘア
	XMFLOAT2 uiPos[3];     // 画面上のクロスヘア位置
};