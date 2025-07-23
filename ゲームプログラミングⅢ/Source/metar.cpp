#include "metar.h"

Metar::Metar()
{
	metar = new Sprite("Data/Sprite/metar2.png");
}

Metar::~Metar()
{
	delete metar;
}

void Metar::update(float gage)
{
	this->gage = gage;
}

void Metar::render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//•`‰æ€”õ
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	metar->Render(rc, 30, 100, 0, 30, this->gage * 5.0f, 0, 1, 0, 0, 1);
}
