#include "MiniMap.h"



void MiniMap::Update(DirectX::XMFLOAT3 playerPosition)
{
	DirectX::XMFLOAT3 camerafront = Camera::Instance().GetFront();
	angle = atan2f(camerafront.x, camerafront.z);

	float screenWidth = static_cast<float>(Graphics::Instance().GetScreenWidth() * Graphics::Instance().GetWindowScaleFactor().x);
	float screenHeight = static_cast<float>(Graphics::Instance().GetScreenHeight() * Graphics::Instance().GetWindowScaleFactor().y);

	spriteWidth = 240;
	spriteHeight = 200;

	iconWidth = 44 * iconsize;
	iconHeight = 64 * iconsize;

	//MapPosition.y = 720 - spriteHeight;
	MapPosition.x = 0;
	MapPosition.y = 720 - spriteHeight;

	iconPosition.x = spriteWidth / 2.0f - iconWidth / 2.0f;
	iconPosition.y = MapPosition.y + spriteHeight / 2.0f - iconHeight / 2.0f;

	cutPosition.x = ((playerPosition.x * (spriteWidth / 60.0f))) - cutsize / 2.0f;
	cutPosition.y = ((-playerPosition.z * (spriteHeight / 50.0f))) - cutsize / 2.0f;
}

void MiniMap::Render(DirectX::XMFLOAT3 playerPosition)
{
	this->playerPosition = playerPosition;
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	//2Dスプライト描画
	{
	/*	float screenWidth = static_cast<float>(1280);
		float screenHeight = static_cast<float>(720);*/
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes



		/*	spriteWidth = 485 ;
			spriteHeight = 411;

			iconWidth = 44 * iconsize;
			iconHeight = 64 * iconsize;

			MapPosition.y = 720 - spriteHeight;

			iconPosition.x = spriteWidth / 2.0f;
			iconPosition.y = MapPosition.y + spriteHeight / 2.0f;*/

		DirectX::XMFLOAT2 offset = { 7.0f,2.5f };

		cutPosition.x = ((playerPosition.x * (spriteWidth / 60))) - cutsize / 2.0f;
		cutPosition.y = ((-playerPosition.z * (spriteHeight / 50))) - cutsize / 2.0f;
		//minimap->Render(rc,
		//	 MapPosition.x,MapPosition.y, 0, spriteWidth, spriteHeight,
		//	spriteWidth/2.0f + cutPosition.x ,spriteHeight/2.0f + cutPosition.y,
		//	cutsize,cutsize,
		//	0,
		//	1, 1, 1, 1,true,radius,parametar);

		minimap->Render(rc,
			MapPosition.x, MapPosition.y, 0, spriteWidth, spriteHeight,
<<<<<<< Updated upstream
<<<<<<< Updated upstream
			spriteWidth / 2.0f + cutPosition.x, spriteHeight / 2.0f + cutPosition.y,
			cutsize, cutsize,
			0,
			1, 1, 1, 1, true, radius, parametar);
=======
			0,0,
			242, 202,
			0,
			1, 1, 1, 1, true, radius, parametar);
=======
			0,0,
			242, 202,
			0,
			1, 1, 1, 1, true, radius, parametar);
>>>>>>> Stashed changes

		//minimap->Render(rc,
		//	MapPosition.x, MapPosition.y, 0, spriteWidth, spriteHeight,
		//	0,
		//	1, 1, 1, 1);
<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes

		//minimap->Render(rc,
		//	MapPosition.x, MapPosition.y, 0, spriteWidth, spriteHeight,
		//	0,
		//	1, 1, 1, 1);

		icon->Render(rc,
			spriteWidth / 2.0f, iconPosition.y, 0, iconWidth, iconHeight,
			DirectX::XMConvertToDegrees(angle),
			0, 0, 1, 1);

		ring->Render(rc, 500, 500, 0, 632, 632, 0, 0, 632, 632, 0, 1, 1, 1, 1);
		//metar->Render(rc, 500, 500, 0, 448, 447, 0, 1, 1, 1, 1);
	}

}

void MiniMap::DrawImGui()
{
	ImVec2 Size = { 200,500 };
	ImGui::SetNextWindowSize(Size);
	if (ImGui::Begin("MiniMap", nullptr))
	{
		ImGui::DragFloat2("MapPosition", &MapPosition.x);
		ImGui::DragFloat("MapSize", &mapsize, 0.01f);
		ImGui::DragFloat("IconSize", &iconsize, 0.01f);
		ImGui::InputFloat2("iconPosition", &iconPosition.x);
		ImGui::InputFloat3("playerPosition", &playerPosition.x);
		ImGui::InputFloat("Angle", &this->angle);
		ImGui::InputFloat2("cutPosition", &cutPosition.x);
		ImGui::DragFloat("cutsize", &cutsize, 1.0f);
		ImGui::DragFloat("radius", &radius);
		ImGui::DragFloat("parametar", &parametar);
	}
	ImGui::End();
}
