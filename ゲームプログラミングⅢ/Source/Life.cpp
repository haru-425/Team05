#include "Life.h"
int Life::life_now_number = 0;

//イージング関数
float easeOutCubic(float t)
{
	float inv = 1.0f - t;
	return 1.0f - inv * inv * inv;
}

Life::Life()
{
	life = new Sprite("Data/Sprite/life.png");
	life_number = life_now_number++;

	position.x = life_number * 30 + 100;
	position.y = 360;
	position.z = 0;
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
	switch (state)
	{
	case 0:
		//position.y += easeOutCubic(time);
		//if (time > 1.0f)
		//{
		//	state++;
		//}
		break;
	case 1:
		break;
	case 2:
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

	life->Render(rc, position.x, position.y, position.z, 198, 135,0,1,1,1,1);
}
