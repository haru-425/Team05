#pragma once
#include "battery.h"
#include "Player/Player.h"
#include "Enemy/Enemy.h"
#include "System/difficulty.h"
#include "System/AudioSource.h"
#include "System/Audio.h"
#include "System/SettingsManager.h"
#include <random>
#include <chrono>
#include <DirectXMath.h>
#include <vector>

#define BATTERY_REMAIN_MAX 9
#define BATTERY_NORMAL_MAX 9
#define BATTERY_HARD_MAX 18
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

	// プレイヤーと敵のバインドを解除
	void BindClear()
	{
		player.reset(); // player 2→1 残りは敵の参照？
		enemy.reset(); // enemy 3→2 どこかわからん
	}

	void SetDifficulty(int diff)
	{
		switch (Difficulty::Instance().GetDifficulty())
		{
		case Difficulty::tutorial:
		case Difficulty::normal:
			battery_recovery = NORML_RECOVERY;
			drop_interval = BATTERY_DROP_NORML_INTERVAL;
			max_Battery = BATTERY_NORMAL_MAX;
			break;
		case Difficulty::hard:
			battery_recovery = HARD_RECOVERY;
			drop_interval = BATTERY_DROP_HARD_INTERVAL;
			max_Battery = BATTERY_HARD_MAX;
			break;
		default:
			break;
		}

		getSE = Audio::Instance().LoadAudioSource("./Data/Sound/get.wav");
	};

	void shuffleBatteryOrder()
	{

		std::random_device rd;
		std::mt19937 gen(rd());

		// Normal と High に振り分けてコピー
		std::vector<BatteryType> normals, highs;
		batteryOrder.reserve(max_Battery);     // メモリを先に確保しておく（無駄な再確保を防ぐ）

		switch (Difficulty::Instance().GetDifficulty())
		{
		case Difficulty::tutorial:
		case Difficulty::normal:
			for (int i = 0; i < max_Battery; ++i) {
				if (normal_Battery_Order[i] == BatteryType::Normal)
					normals.push_back(normal_Battery_Order[i]);
				else
					highs.push_back(normal_Battery_Order[i]);
			}
			break;
		case Difficulty::hard:
			for (int i = 0; i < max_Battery; ++i) {
				if (hard_Battery_Order[i] == BatteryType::Normal)
					normals.push_back(hard_Battery_Order[i]);
				else
					highs.push_back(hard_Battery_Order[i]);
			}
			break;
		default:
			break;
		}

		// 配置し直し
		for (int i = 0; i < max_Battery; ++i) {
			double prob_high = static_cast<double>(i) / (max_Battery - 1); // 後ろに行くほどHigh確率UP
			std::uniform_real_distribution<> dist(0.0, 1.0);
			bool pick_high = dist(gen) < prob_high;

			if (pick_high && !highs.empty()) {
				// High をランダムに取る
				std::uniform_int_distribution<> d(0, highs.size() - 1);
				int idx = d(gen);
				batteryOrder.push_back(highs[idx]);
				highs.erase(highs.begin() + idx);
			}
			else if (!normals.empty()) {
				// Normal をランダムに取る
				std::uniform_int_distribution<> d(0, normals.size() - 1);
				int idx = d(gen);
				batteryOrder.push_back(normals[idx]);
				normals.erase(normals.begin() + idx);
			}
			else if (!highs.empty()) {
				// Normal が尽きたら High を詰める
				batteryOrder.push_back(highs.back());
				highs.pop_back();
			}
		}
	}

	void Update(float elapsedTime)
	{
		droptime += elapsedTime;
		for (auto& battery : hasBattery)
		{
			battery.Update(elapsedTime);
		}
		if (hasBattery.size() > max_Battery && !hasBattery.empty())
		{
			hasBattery.erase(hasBattery.begin());
		}
		deleteBattery(player->GetPosition());
		if (droptime >= drop_interval && dropFlag)
		{
			addBattery(enemy->GetPosition());
			droptime = 0;
		}
		GameSettings setting = SettingsManager::Instance().GetGameSettings();
		getSE->SetVolume(0.5f * setting.seVolume * setting.masterVolume);
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
		if (drop_Count + 1 > max_Battery)
		{
			return;
		}
		hasBattery.push_back(Battery(batteryOrder[drop_Count]));
		hasBattery.back().setPos(pos);
		switch (batteryOrder[drop_Count])
		{
		case BatteryType::Normal:
			hasBattery.back().setModel(normal_Battery_Model);
			break;
		case BatteryType::High:
			hasBattery.back().setModel(hard_Battery_Model);
			break;
		default:
			break;
		}
		drop_Count++;
	}

	void deleteBattery(DirectX::XMFLOAT3 pos)
	{
		for (auto it = hasBattery.begin(); it != hasBattery.end();)
		{
			if (DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&it->getPos()), DirectX::XMLoadFloat3(&pos)))) < 1.0f)
			{
				getSE->Play(false);
				player_Get_Battery.push_back(it->getType());
				switch (it->getType())
				{
				case BatteryType::Normal:
					player_Get_Score += 10;
					break;
				case BatteryType::High:
					player_Get_Score += 20;
					break;
				default:
					break;
				}
				hasBattery.erase(it);
				player->AddHijackTimer(battery_recovery);
				break;
			}
			else
			{
				++it;
			}
		}
	}

	void SetPlayer_and_Enemy(std::shared_ptr<Player> players, std::shared_ptr<Enemy> enemys) { player = players; enemy = enemys; };

	void ClearBattery() {
		if (!hasBattery.empty())
		{
			hasBattery.clear();
		}
		if (!player_Get_Battery.empty())
		{
			player_Get_Battery.clear();
		}
		drop_Count = 0;
		max_Score = 0;
		player_Get_Score = 0;
		droptime = 0;
	}
	int getMax_Score()
	{
		return max_Score;
	}

	int getScore()
	{
		return player_Get_Score;
	}

	int getPlayerHasBattery(int i)
	{
		if (player_Get_Battery.size() <= i)
		{
			return -1;
		}

		return player_Get_Battery[i];
	}

	void stop(){ dropFlag = false; }

	void start(){ dropFlag = true; }

private:

	batteryManager() {}
	~batteryManager() {}

	BatteryType normal_Battery_Order[BATTERY_NORMAL_MAX] = { Normal,Normal,Normal,Normal,Normal,Normal,High,High,High };
	BatteryType hard_Battery_Order[BATTERY_HARD_MAX] = { Normal,Normal,Normal,Normal,Normal,Normal,Normal,Normal,Normal,Normal,High,High,High,High,High,High,High,High };

	std::vector<BatteryType> batteryOrder;

	std::vector<Battery> hasBattery;

	std::vector<BatteryType> player_Get_Battery;

	std::shared_ptr<Model> hard_Battery_Model = std::make_shared<Model>("Data/Model/battery_assets/battery_geo.mdl");
	std::shared_ptr<Model> normal_Battery_Model = std::make_shared<Model>("Data/Model/battery_assets/battery_normal.mdl");

	std::shared_ptr<Player> player;

	std::shared_ptr<Enemy> enemy;

	int drop_Count = 0;

	int player_Get_Score = 0;

	int max_Score = 0;

	int max_Battery = 0;

	float battery_recovery = NORML_RECOVERY;

	float drop_interval = BATTERY_DROP_NORML_INTERVAL;

	float droptime = 0;

	bool dropFlag = true;

	AudioSource* getSE;
};