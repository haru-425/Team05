#pragma once

#include <vector>
#include <DirectXMath.h>

#include "System/RenderContext.h"
#include "System/ShapeRenderer.h"

class LightManager
{
public:
	// インスタンス取得
	static LightManager& Instance()
	{
		static LightManager instance;
		return instance;
	}

private:
	// 点光源構造体（位置・色・範囲）
	struct PointLightConstants
	{
		DirectX::XMFLOAT4 position;  // ワールド座標（w未使用）
		DirectX::XMFLOAT4 color;     // 光の色
		float range;                 // 有効距離（減衰終了距離）
	};

	// 線光源構造体（始点・終点・色・範囲）
	struct LineLightConstants
	{
		DirectX::XMFLOAT4 start;     // ワールド空間での始点
		DirectX::XMFLOAT4 end;       // ワールド空間での終点
		DirectX::XMFLOAT4 color;     // 光の色
		float range;                 // 有効距離（線との距離で減衰）
	};

	// 光源位置・向きを格納する汎用データ（カスタム用途など）
	struct LightData
	{
		DirectX::XMFLOAT3 position;  // ライトの位置
		float             angle;     // 向きや回転（未使用ならゼロ）
		float             length;    // 光源の長さ
	};

public:
	LightManager() = default;
	~LightManager() = default;

	void Initialize();
	void Update();
	void RenderDebug(RenderContext& rc);

	void DebugGUI();

	void UpdateConstants(RenderContext& rc);

	void PointLightDebug(char* c,int begin,int end);
	void LineLightDebug(char* c, int begin,int end);

	const std::vector<PointLightConstants>& GetPointLights() const { return pointLights; }
	const std::vector<LineLightConstants>& GetLineLights() const { return lineLights; }


private:
	// 光源情報
	std::vector<PointLightConstants> pointLights;
	std::vector<LineLightConstants>  lineLights;
	std::vector<LightData>           lightData;;

	// 全体のライト強度（乗算係数）
	float lightPower = 5.0f;

	struct LightColors
	{
		DirectX::XMFLOAT4 red    = { 1,0,0,1 };
		DirectX::XMFLOAT4 blue   = { 0,0,1,1 };
		DirectX::XMFLOAT4 yellow = { 1,1,0,1 };
		DirectX::XMFLOAT4 green  = { 0,1,0,1 };
		DirectX::XMFLOAT4 purple = { 1,0,1,1 };
	};
	LightColors lightColor;

#define POINTLIGHT_MAX     47
#define LINELIGHT_MAX      42

#define POINTLIGHT_S_RANGE  6
#define POINTLIGHT_L_RANGE  8
#define LINELIGHT_RANGE     5
#define CEILNG_HEIGHT       3

};

