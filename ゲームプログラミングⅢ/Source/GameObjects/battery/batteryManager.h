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

    void Update(float elapsedTime)
    {
        for (auto& battery : hasBattery)
        {
            battery.Update(elapsedTime);
        }
        if (hasBattery.size() >= BATTERY_MAX && !hasBattery.empty())
        {
            hasBattery.erase(hasBattery.begin());
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

    std::shared_ptr<Model> batterymodel = std::make_shared<Model>("Data/Model/battery_assets/battery_geo.mdl");


};