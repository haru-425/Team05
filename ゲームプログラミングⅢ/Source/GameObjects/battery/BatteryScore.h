#pragma once

#include "System/RenderContext.h"
#include "System/Sprite.h"

#include <memory>

class BatteryScore
{
public:
	// インスタンス取得
	static BatteryScore& Instance()
	{
		static BatteryScore instance;
		return instance;
	}

	BatteryScore() {}
	~BatteryScore() {}

	void Initialize();
	void Finalize();

	void Update(float elapsedTime);
	void Render(RenderContext& rc);

private:
	enum class batteryPattern
	{
		normal  = 0,
		spetial = 1,

		none    = -1,
	};

	const int MAX_BATTERY = 10;
	std::unique_ptr<Sprite> sprites[2];
};

