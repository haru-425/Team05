#pragma once
#include"System/sprite.h"
#include"System/Graphics.h"

class Metar
{
public:

	Metar();

	~Metar();


	void update(float gage);

	void render();
private:
	Sprite* metar;
	Sprite* frame;

	float gage = 100;
};