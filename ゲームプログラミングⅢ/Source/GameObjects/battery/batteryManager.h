#pragma once
#include "battery.h"
#include <DirectXMath.h>
#include <vector>

#define BATTERY_MAX 10

class batteryManager
{
public:
    // インスタンス取得
    static batteryManager& Instance() {
        static batteryManager instance;
        return instance;
    }

    void update() 
    {
        for (auto battery : hasBattery)
        {
            battery.update();
        }
        if (hasBattery.size() >= BATTERY_MAX && !hasBattery.empty())
        {
            hasBattery.erase(hasBattery.begin());
        }
    }

    void render()
    {
        for (auto battery : hasBattery)
        {
            battery.render();
        }
    }

    void addBattery(DirectX::XMFLOAT3 pos)
    {
		hasBattery.push_back(battery(pos));
    }

    void deleteBattery(DirectX::XMFLOAT3 pos)
    {
		for (auto it = hasBattery.begin(); it != hasBattery.end();)
		{
			if (DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&it->getPos()), DirectX::XMLoadFloat3(&pos)))) < 0.5f)
			{
				it = hasBattery.erase(it);
                break;
			}
			else
			{
				++it;
			}
		}
    }
private:
    batteryManager() {}
    ~batteryManager() {}

    std::vector<battery> hasBattery;


};