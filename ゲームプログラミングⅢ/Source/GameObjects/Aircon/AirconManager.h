#pragma once

#include <vector>
#include <DirectXMath.h>

#include "System/RenderContext.h"
#include "System/ModelRenderer.h"

#include "Aircon.h"

class AirconManager
{
private:
	std::vector<std::unique_ptr<Aircon>> aircons;

#define AIRCON_MAX          13

#define LIGHT_HEIGHT       2.999f
public:
	// インスタンス取得
	static AirconManager& Instance()
	{
		static AirconManager instance;
		return instance;
	}

	AirconManager() = default;
	~AirconManager() = default;

	void Initialize();
	void Update();

	void Render(RenderContext& rc);
	void RenderDebugPrimitive(RenderContext& rc);

	void DebugGUI();
	DirectX::XMFLOAT3 AirconManager::GetAirconPosition(size_t index) const
	{
		if (index >= aircons.size() || !aircons[index]) {
			return { 0.0f, 0.0f, 0.0f }; // 不正なインデックス時のデフォルト値
		}

		return aircons[index]->GetPosition();
	}


};

