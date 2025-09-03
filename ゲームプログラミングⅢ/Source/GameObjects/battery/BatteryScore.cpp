#include "BatteryScore.h"
#include "batteryManager.h"

void BatteryScore::Initialize()
{
	/// �X�v���C�g�̓ǂݍ���
	sprites[static_cast<int>(batteryPattern::normal)] = std::make_unique<Sprite>("Data/Sprite/battery_icon/battery_icon_normal.png");
	sprites[static_cast<int>(batteryPattern::spetial)] = std::make_unique<Sprite>("Data/Sprite/battery_icon/battery_icon_high.png");
}

void BatteryScore::Finalize()
{

}
void BatteryScore::Update(float elapsedTime)
{

}
void BatteryScore::Render(RenderContext& rc)
{
	float size = 40;
	float posX = 50 + size;
	float posY = 450;

	for (int i = 0; i < 32; ++i)
	{
		int pattern = batteryManager::Instance().getPlayerHasBattery(i);

		/// null�̏ꍇ�͕`�悵�Ȃ�
		if (pattern == -1) break;

		/// �l������10�𒴂����玟�̗��
		if (i == 10) { 
			posX += size; 
			posY = 450;
		}

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

		posY -= (size + 8);
	}
}