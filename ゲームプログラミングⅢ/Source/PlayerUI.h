#pragma once

#pragma once
#include <DirectXMath.h>
#include "System/Sprite.h"
#include "System/RenderContext.h"
#include "Camera/Camera.h"
#include "System/Graphics.h"
#include <random>

using namespace DirectX;

class PlayerUI {
public:
	static PlayerUI& Instance() {
		static PlayerUI instance;
		return instance;
	}


	void Initialize() {
		ui = new Sprite("Data/Sprite/player_crosshair.png");


	}

	void Finalize() {
		delete ui;

	}

	void Update(float elapsedTime) {

	}

	void Render(const RenderContext& rc) {
		float scaleX = Graphics::Instance().GetWindowScaleFactor().x;
		float scaleY = Graphics::Instance().GetWindowScaleFactor().y;

		ui->Render(
			rc,
			0,
			0,
			0.0f,
			1280,
			720,
			0,
			1.0f, 1.0f, 1.0f, 1.0f
		);

	}

private:
	Sprite* ui;         // ÉNÉçÉXÉwÉA
};