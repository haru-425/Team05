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

//void Metar::render()
//{
//	Graphics& graphics = Graphics::Instance();
//	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
//	RenderState* renderState = graphics.GetRenderState();
//
//	//•`‰æ€”õ
//	RenderContext rc;
//	rc.deviceContext = dc;
//	rc.renderState = graphics.GetRenderState();
//
//	metar->Render(rc, 30, 100, 0, 30, this->gage * 5.0f, 0, 0.3f, 0.3f, 1, 0.5f);
//	frame->Render(rc, 30, 100, 0, 40, 250.0f, 0, 0, 0, 1, 1);
//}
//void Metar::render()
//{
//	Graphics& graphics = Graphics::Instance();
//	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
//	RenderState* renderState = graphics.GetRenderState();
//
//	// •`‰æ€”õ
//	RenderContext rc;
//	rc.deviceContext = dc;
//	rc.renderState = graphics.GetRenderState();
//
//	float maxHeight = 250.0f; // ƒQ[ƒW‚ÌÅ‘å‚‚³
//	float currentHeight = this->gage * 5.0f;
//	float baseY = 30.0f;
//	float adjustedY = baseY + (maxHeight - currentHeight); // ã‚©‚ç‰º‚ÉŒ¸­‚³‚¹‚é‚½‚ß‚ÌYÀ•W’²®
//
//	metar->Render(rc, 30, adjustedY, 0, 30, currentHeight, 0, 0.3f, 0.3f, 1, 0.5f);
//	frame->Render(rc, 30, baseY, 0, 40, maxHeight, 0, 0, 0, 1, 1);
//}

void Metar::render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	// •`‰æ€”õ
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	float maxHeight = 250.0f;
	float currentHeight = this->gage * 5.0f;

	// ’†‰›Šñ‚è‚ÌÀ•W‚É•ÏX
	float posX = 50.0f;
	float baseY = 100.0f;
	float adjustedY = baseY + (maxHeight - currentHeight);

	metar->Render(rc, posX, adjustedY, 0, 30, currentHeight, 0, 1, 1, 1, 0.5f);
	frame->Render(rc, posX, baseY, 0, 40, maxHeight, 0, 1, 1, 1, 1);
}