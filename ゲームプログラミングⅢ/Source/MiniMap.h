#pragma once
#include"System/Graphics.h"
#include"System/Sprite.h"
#include "imgui.h"
#include"Camera/Camera.h"


class MiniMap
{
public:
	MiniMap() {
		minimap = new Sprite("Data/Sprite/Map.png");
		icon = new Sprite("Data/Sprite/icon.png");
		metar = new Sprite("Data/Sprite/metar.png");
		ring = new Sprite("Data/Sprite/ring.png");

	}
	~MiniMap() {
		delete minimap;
		delete icon;
		delete metar;
		delete ring;
	}

	void Update(DirectX::XMFLOAT3 playerPosition);

	void Render(DirectX::XMFLOAT3 playerPosition);

	void DrawImGui();

private:
	Sprite* minimap = nullptr;
	Sprite* icon = nullptr;
	Sprite* metar = nullptr;
	Sprite* ring = nullptr;

	DirectX::XMFLOAT2 MapPosition = { 0,720 - 200 };
	//DirectX::XMFLOAT2 MapPosition = { 50,720 - 200 };
	DirectX::XMFLOAT2 iconPosition = { 0,0 };
	DirectX::XMFLOAT3 playerPosition = { 0,0,0 };
	DirectX::XMFLOAT2 cutPosition = { 0 , 0 };

	float radius = 100.0f;

	float cutsize = 100.0f;
	float mapsize = 0.75f;
	float iconsize = 0.35f;
	float angle;

	float spriteWidth = 240;
	float spriteHeight = 200;

	float iconWidth = 44;
	float iconHeight = 64;

	float parametar = 360.0f;

};