#pragma once
#include"System/sprite.h"
#include"System/Graphics.h"

class Life
{
public:
	Life();
	~Life();

	void  Update(float elapsed_Time);

	void Render();

private:
	Sprite* life;

	//自分の番号
	int life_number;

	//現在のライフ
	static int life_now_number;

	//今のステイと
	int state = 0;

	//時間
	float time;

	DirectX::XMFLOAT3 position;
};