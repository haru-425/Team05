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

	//�����̔ԍ�
	int life_number;

	//���݂̃��C�t
	static int life_now_number;

	//���̃X�e�C��
	int state = 0;

	//����
	float time;

	float max_time = 0.01f;

	//�����ꋭ��
	float strength = 0;

	//�o��
	
	bool flag;

	bool delete_life;

	//�J�E���g
	float count = 0;

	float state_time = 0;


	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 shake_position = { 0,0,0 };
	DirectX::XMFLOAT3 OutElastic = { 0,0,0 };
	float alpha = 0;
};