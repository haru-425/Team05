#include "metar.h"

Metar::Metar()
{
	metar = new Sprite("Data/Sprite/metar2.png");
	frame = new Sprite("Data/Sprite/metarFrame.png");
}

Metar::~Metar()
{
	delete metar;
	delete frame;
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

	metar->Render(rc, 30, 100, 0, 30, this->gage * 5.0f, 0, 0.1f, 0.1f, 10, 0.5f);
	frame->Render(rc, 30, 100, 0, 40, 250.0f, 0, 0, 0, 10, 1);
}
