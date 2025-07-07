#include "SceneGameOver.h"


void Game_Over::Initialize()
{
	GameOver = new Sprite("Data/Sprite/GameOver.png");
	for (int i = 0; i < 3; i++)
	{
		life[i] = new Life();
	}
}

void Game_Over::Finalize()
{
	for (int i = 0; i < 3; i++)
	{
		if (life[i] != nullptr)
		{
			delete life[i];
		}
		life[i] = nullptr;
	}
	if (GameOver != nullptr)
	{
		delete GameOver;
		GameOver = nullptr;
	}
}

void Game_Over::Update(float elapsedTime)
{
}

void Game_Over::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//•`‰æ€”õ
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();
	GameOver->Render(rc,100, 100, 0, 1095, 316, 0, 1, 1,1,1);

	for (int i = 0; i < 3; i++)
	{
		if (life[i] != nullptr)
		{
			life[i]->Render();
		}
	}
}
