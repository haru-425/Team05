#pragma once
#include "battery.h"
#include "Player/Player.h"
#include "Enemy/Enemy.h"
#include "System/difficulty.h"
#include <chrono>
#include <DirectXMath.h>
#include <vector>

#define BATTERY_MAX 10
#define BATTERY_DROP_NORML_INTERVAL 20.0f
#define BATTERY_DROP_HARD_INTERVAL 10.0f
#define NORML_RECOVERY 10.0f
#define HARD_RECOVERY 5.0f

class batteryManager
{
public:
	// インスタンス取得
	static batteryManager& Instance() {
		static batteryManager instance;
		return instance;
	}

	void SetDifficulty(int diff)
	{
		if (Difficulty::Instance().GetDifficulty() == Difficulty::normal)
		{
			battery_recovery = NORML_RECOVERY;
			drop_interval = BATTERY_DROP_NORML_INTERVAL;
		}
		else if (Difficulty::Instance().GetDifficulty() == Difficulty::hard)
		{
			battery_recovery = HARD_RECOVERY;
			drop_interval = BATTERY_DROP_HARD_INTERVAL;
		}
	};

	void Update(float elapsedTime)
	{
		droptime += elapsedTime;
		for (auto& battery : hasBattery)
		{
			battery.Update(elapsedTime);
		}
		if (hasBattery.size() > BATTERY_MAX && !hasBattery.empty())
		{
			hasBattery.erase(hasBattery.begin());
		}
		deleteBattery(player->GetPosition());
		if (droptime >= drop_interval)
		{
			addBattery(enemy->GetPosition());
			droptime = 0;
		}
	}

	void Render(const RenderContext& rc, ModelRenderer* renderer)
	{
		for (auto battery : hasBattery)
		{
			battery.Render(rc, renderer);
		}
	}

	void addBattery(DirectX::XMFLOAT3 pos)
	{
		hasBattery.push_back(battery());
		hasBattery.back().setPos(pos);
		hasBattery.back().setModel(batterymodel);

		game_Max_Batterry++;
	}

	void deleteBattery(DirectX::XMFLOAT3 pos)
	{
		for (auto it = hasBattery.begin(); it != hasBattery.end();)
		{
			if (DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&it->getPos()), DirectX::XMLoadFloat3(&pos)))) < 0.5f)
			{
				it = hasBattery.erase(it);
				player_Get_Batterry++;
				player->AddHijackTimer(battery_recovery);
				break;
			}
			else
			{
				++it;
			}
		}
	}

	void SetPlayer_and_enemy(std::shared_ptr<Player> players, std::shared_ptr<Enemy> enemys) { player = players; enemy = enemys; };

	void ResetPlayer_get_Batterry() { player_Get_Batterry = 0; };
	int getMAXBattery()
	{

		return game_Max_Batterry;
	}
	int getPlayerHasBattery()
	{
		return player_Get_Batterry;
	}
private:
	batteryManager() {}
	~batteryManager() {}

	std::vector<battery> hasBattery;

	std::shared_ptr<Model> batterymodel = std::make_shared<Model>("Data/Model/battery_assets/battery_geo.mdl");

	std::shared_ptr<Player> player;

	std::shared_ptr<Enemy> enemy;

	int player_Get_Batterry = 0;

	int game_Max_Batterry = 0;

	float battery_recovery = NORML_RECOVERY;

	float drop_interval = BATTERY_DROP_NORML_INTERVAL;

	float droptime = 0;
};