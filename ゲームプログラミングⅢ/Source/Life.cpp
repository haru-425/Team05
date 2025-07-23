#include "Life.h"
int Life::life_now_number = 0;

//イージング関数
float easeOutCubic(float t)
{
	float inv = 1.0f - t;
	return 1.0f - inv * inv * inv;
}

int shake(float count)
{
	int random = (rand() % 10 * count) - 5 * count;
	return random;
}

float easeInCubic(float t)
{
	return t * t * t;
}

float easeOutElastic(float t)
{
	if (t == 0.0f) return 0.0f;
	if (t >= 1.0f) return 1.0f;

	float amplitude = 0.15f;      // バウンドの大きさを小さく
	float period = 0.5f;         // 振動の速さ（そのまま）
	float tau = 6.2831853f;      // 2π

	return amplitude * pow(1.0f, -1.0f * t) * sin((t - period / 4.0f) * (tau / period)) + 1.0f;
}

float Lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}


Life::Life(int life)
{
	this->life = new Sprite("Data/Sprite/life.png");
	life_number = life_now_number++;

	switch (life)
	{
	case 0:
		position.x = life_number * 200 -600;
		position.y = 600;
		position.z = 0;
		break;
	case 1:
		position.x = life_number * 200 -200;
		position.y = 600;
		position.z = 0;
		break;
	case 2:
		position.x = life_number * 200 + 300;
		position.y = 600;
		position.z = 0;
		break;
	}
}

Life::~Life()
{
	if (life != nullptr)
	{
		delete life;
		life = nullptr;
	}
}

void Life::Update(float elapsed_Time)
{
	float a = strength * strength * strength;
	life->SetNoise(1);
	life->SetStrength(a);
	switch (state)
	{
	case 0:
		if (time > 0.1f * life_number)
		{
			flag = true;
		}
		if (flag)
		{
			position.y = Lerp(600, 360, easeInCubic(count));
			count += 0.02f;
			if (count >= 0.9f)
			{
				state++;
				count = 0;
			}
		}
		break;
	case 1:
	{
		if (count > 2.5f)
		{
			state++;
		}
		count++;
		break;
	}
	case 2:
		state_time += 0.01f;
		if (delete_life)
		{
			state++;
		}
		break;
	case 3:
		if (strength > 1.6f)
		{
			state = 4;
			strength = 1;
		}
		strength += 0.05f;
		if (strength * 2 > 1.6f)
		{
			shake_position.x = shake(5);
			shake_position.y = shake(5);
		}
		break;
	}
	time += 0.01f;
}

void Life::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	if (state == 4)return;
	life->Render(rc, position.x + shake_position.x, position.y + shake_position.y - OutElastic.y, position.z, 198, 135,0,1,1,1,0);
}
