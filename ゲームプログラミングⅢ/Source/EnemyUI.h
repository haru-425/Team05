#pragma once
#include <DirectXMath.h>
#include "System/Sprite.h"
#include "System/RenderContext.h"
#include "Camera/Camera.h"
#include "System/Graphics.h"
#include <random>

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
		adjustedPos.y += 1.0f;  // ���������i�K�v�ɉ����āj
		XMVECTOR pos = XMLoadFloat3(&adjustedPos);

		XMMATRIX vp = XMMatrixMultiply(view, proj);
		XMVECTOR projected = XMVector3TransformCoord(pos, vp);

		// ��ʂ̒���
		XMFLOAT2 center(viewportWidth * 0.5f, viewportHeight * 0.5f);

		// ����O�Ȃ��ʒ�����Ԃ�
		if (projected.m128_f32[2] < 0.0f || projected.m128_f32[2] > 1.0f ||
			projected.m128_f32[0] < -1.0f || projected.m128_f32[0] > 1.0f ||
			projected.m128_f32[1] < -1.0f || projected.m128_f32[1] > 1.0f)
		{
			return center;
		}

		// NDC �� �X�N���[�����W��
		float x = (projected.m128_f32[0] + 1.0f) * 0.5f * viewportWidth;
		float y = (1.0f - projected.m128_f32[1]) * 0.5f * viewportHeight;

		return XMFLOAT2(x, y);
	}

	void Initialize() {
		ui[0] = new Sprite("Data/Sprite/crosshair1.png");
		ui[1] = new Sprite("Data/Sprite/crosshair2.png");
		ui[2] = new Sprite("Data/Sprite/crosshair3.png");

		for (int i = 0; i < 3; ++i) {
			uiPos[i] = XMFLOAT2(640.0f, 360.0f); // �����ʒu�F��ʒ���
		}
	}

	void Finalize() {
		for (int i = 0; i < 3; ++i) {
			delete ui[i];
		}
	}
	XMFLOAT2 GetRandomScreenPos() {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_real_distribution<float> distX(0.0f, static_cast<float>(Graphics::Instance().GetScreenWidth()));
		std::uniform_real_distribution<float> distY(static_cast<float>(Graphics::Instance().GetScreenHeight() / 4.f), static_cast<float>(Graphics::Instance().GetScreenHeight() / 4.f * 3));
		return XMFLOAT2(distX(gen), distY(gen));
	}
	void Update(float elapsedTime, const XMFLOAT3& playerWorldPos, bool playerDetected) {
		static float timer = 0.0f;
		static XMFLOAT2 randomTarget = GetRandomScreenPos();
		timer += elapsedTime;

		XMFLOAT2 targetPos;
		if (playerDetected) {
			targetPos = WorldToScreen(
				playerWorldPos,
				Camera::Instance().GetView(),
				Camera::Instance().GetProjection(),
				Graphics::Instance().GetScreenWidth(),
				Graphics::Instance().GetScreenHeight()
			);
			timer = 0.0f; // �^�[�Q�b�g�����Z�b�g
		}
		else {
			if (timer >= 1.0f) { // 1�b���ƂɃ����_���ɐ؂�ւ�
				randomTarget = GetRandomScreenPos();
				timer = 0.0f;
			}
			targetPos = randomTarget;
		}

		// �擪�N���X�w�A�F�v���C���[�܂��̓����_���ʒu�ɒǏ]
		{
			float speed = std::clamp(6.0f * elapsedTime, 0.0f, 1.0f);
			XMVECTOR current = XMLoadFloat2(&uiPos[0]);
			XMVECTOR target = XMLoadFloat2(&targetPos);
			XMVECTOR result = XMVectorLerp(current, target, speed);
			XMStoreFloat2(&uiPos[0], result);
		}

		// �㑱�N���X�w�A�F�ЂƂO�ɒǏ]
		for (int i = 1; i < 3; ++i) {
			float rawSpeed = 20.0f - i * 2.0f;
			float followSpeed = std::clamp(rawSpeed * elapsedTime, 0.0f, 1.0f);

			XMVECTOR current = XMLoadFloat2(&uiPos[i]);
			XMVECTOR target = XMLoadFloat2(&uiPos[i - 1]);
			XMVECTOR result = XMVectorLerp(current, target, followSpeed);
			XMStoreFloat2(&uiPos[i], result);
		}
	}

	void Render(const RenderContext& rc) {
		float scaleX = Graphics::Instance().GetWindowScaleFactor().x * 0.5F;
		float scaleY = Graphics::Instance().GetWindowScaleFactor().y * 0.5F;
		for (int i = 0; i < 3; ++i) {
			ui[i]->Render(
				rc,
				(uiPos[i].x - (160 / 2 /*- i * 20*/) /** scaleX */ / 2.f),
				(uiPos[i].y - (160 / 2 /*- i * 20*/) /** scaleY*/ / 2.f),
				0.0f,
				(160 /*- i * 20*/) / 2 /** scaleX*/,
				(160 /*- i * 20*/) / 2/* * scaleY*/,
				0,
				1.0f, 1.0f, 1.0f, 1.0f
			);
		}
	}

private:
	Sprite* ui[3];         // �N���X�w�A
	XMFLOAT2 uiPos[3];     // ��ʏ�̃N���X�w�A�ʒu
};