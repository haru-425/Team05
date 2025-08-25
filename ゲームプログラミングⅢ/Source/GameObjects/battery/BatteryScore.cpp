#include "BatteryScore.h"

void BatteryScore::Initialize()
{
	/// スプライトの読み込み
	sprites[static_cast<int>(batteryPattern::normal)] = std::make_unique<Sprite>("Data/Sprite/battery_icon_normal.png");
	sprites[static_cast<int>(batteryPattern::spetial)] = std::make_unique<Sprite>("Data/Sprite/battery_icon_high.png");
}

void BatteryScore::Finalize()
{

}
void BatteryScore::Update(float elapsedTime)
{

}
void BatteryScore::Render(RenderContext& rc)
{
	float size = 64;

	for (int i = 0; i < MAX_BATTERY; ++i)
	{
		float posX = 1280 - size;
		float posY = 600;

		int pattern = 0;
		/// int pattern = getPlayerHasBattery(i);

		if (pattern == -1) break;

		switch (pattern) {
		case static_cast<int>(batteryPattern::normal):
			sprites[static_cast<int>(batteryPattern::normal)]->Render(
				rc, posX, posY, 0, size, size, 0, 0, 256, 256, 0, 1, 1, 1, 1);

			break;
		case static_cast<int>(batteryPattern::spetial):
			sprites[static_cast<int>(batteryPattern::spetial)]->Render(
				rc, posX, posY, 0, size, size, 0, 0, 256, 256, 0, 1, 1, 1, 1);

			break;
		}

		posY -= (size + 20);
	}
}