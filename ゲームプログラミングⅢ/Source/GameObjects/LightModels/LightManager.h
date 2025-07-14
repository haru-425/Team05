#pragma once

#include <vector>
#include <DirectXMath.h>

#include "System/RenderContext.h"
#include "System/ShapeRenderer.h"
#include "System/ModelRenderer.h"

#include "./LightModels/LightPoint/LightPoint.h"
#include "./LightModels/LightBar/LightBar.h"
#include "./LightModels/LightTorus/LightTorus.h"

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
	struct TorusLightConstants
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 direction;
		DirectX::XMFLOAT4 color;
		float majorRadius;   // ドーナツの中心円の半径
		float minorRadius;   // ドーナツの太さ（断面円の半径）
		float range;
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

	void Render(RenderContext& rc);
	void RenderDebugPrimitive(RenderContext& rc);

	void DebugGUI();

	void UpdateConstants(RenderContext& rc);

	void PointLightDebug(char* c,int begin,int end);
	void TorusLightDebug(char* c, int begin,int end);
	void LineLightDebug(char* c, int begin,int end);

	const std::vector<PointLightConstants>& GetPointLights() const { return pointLights; }
	const std::vector<LineLightConstants>& GetLineLights() const { return lineLights; }


private:
	// 光源情報
	std::vector<PointLightConstants> pointLights;
	std::vector<TorusLightConstants> torusLights;
	std::vector<LineLightConstants>  lineLights;
	std::vector<LightData>           lightData;

	// モデル
	std::vector<std::unique_ptr<LightPoint>>  pointLightsModel;
	std::vector<std::unique_ptr<LightTorus>>  torusLightsModel;
	std::vector<std::unique_ptr<LightBar>>    lineLightsModel;

	// 全体のライト強度（乗算係数）
	float lightPower = 5.0f;

	struct LightColors
	{
		DirectX::XMFLOAT4 red    = { 0.5f,    0.0f   , 0.0f    ,1 };
		DirectX::XMFLOAT4 blue   = { 0.0f,    0.0664f, 0.3242f ,1 };
		DirectX::XMFLOAT4 yellow = { 1.0f,    0.9453f, 0.0f    ,1 };
		DirectX::XMFLOAT4 green  = { 0.2578f ,0.2578f, 0.0f    ,1 };
		DirectX::XMFLOAT4 purple = { 0.3242f ,0.0f   , 0.4492f ,1 };
	};
	LightColors lightColor;

#define POINTLIGHT_MAX     34
#define TORUSLIGHT_MAX     13
#define LINELIGHT_MAX      45

#define POINTLIGHT_RANGE    5.5f

#define TORUSLIGHT_RANGE    6
#define TORUSLIGHT_MAJOR    5.5
#define TORUSLIGHT_MINOR    0.2

#define LINELIGHT_RANGE     5

#define CEILNG_HEIGHT       2.999f

	std::unique_ptr<Model> models[3];
	std::unique_ptr<LoadTextures> textures[3];
};

