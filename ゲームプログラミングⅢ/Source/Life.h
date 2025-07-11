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

	void SetFlag(bool flag) { delete_life = flag; }

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

	float max_time = 0.01f;

	//横ずれ強さ
	float strength = 0;

	//出現
	
	bool flag;

	bool delete_life;

	//カウント
	float count = 0;

	float state_time = 0;


	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 shake_position = { 0,0,0 };
	DirectX::XMFLOAT3 OutElastic = { 0,0,0 };
	float alpha = 0;
};