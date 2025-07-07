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

	//�����̔ԍ�
	int life_number;

	//���݂̃��C�t
	static int life_now_number;

	//���̃X�e�C��
	int state = 0;

	//����
	float time;

	DirectX::XMFLOAT3 position;
};