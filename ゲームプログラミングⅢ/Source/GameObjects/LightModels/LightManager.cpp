#include "LightManager.h"

#include <imgui.h>
#include "System/Graphics.h"
#include <iostream>
#include "System/Misc.h"
#include "System/LoadTextures.h"

void LightManager::Initialize()
{
	Benchmark bench;
	bench.begin();
	// ライトの強さ
	lightPower = 3;

	// 点光源の初期化
	{
		pointLights.clear();

		// 青ライト ------------------------------------------------------------------------------------------------------
		/* 最奥部屋 */
		pointLights.emplace_back(PointLightConstants{ {-1.3f, CEILNG_HEIGHT, 23.5f,.0f},{lightColor.blue} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-1.3f, CEILNG_HEIGHT, 20.5f,.0f},{lightColor.blue} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {2.3f,  CEILNG_HEIGHT, 23.5f,.0f},{lightColor.blue} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {2.3f,  CEILNG_HEIGHT, 20.5f,.0f},{lightColor.blue} , POINTLIGHT_RANGE });

		/* 大ライト通路 */
		pointLights.emplace_back(PointLightConstants{ {-0.1f, CEILNG_HEIGHT, -10.8f,.0f},{lightColor.blue} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {2.25f, CEILNG_HEIGHT, -10.8f,.0f},{lightColor.blue} , POINTLIGHT_RANGE });

		// 赤ライト ------------------------------------------------------------------------------------------------------
	   /* 3席 */
		pointLights.emplace_back(PointLightConstants{ {-29.0f, CEILNG_HEIGHT, 13.0f,.0f},{lightColor.red} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-29.0f, CEILNG_HEIGHT, 5.0f,.0f}, {lightColor.red} , POINTLIGHT_RANGE });

		/* 1席 */
		pointLights.emplace_back(PointLightConstants{ { 29.0f, CEILNG_HEIGHT, 13.0f,.0f},{lightColor.red} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ { 29.0f, CEILNG_HEIGHT, 5.0f,.0f}, {lightColor.red} , POINTLIGHT_RANGE });

		/* OBJ角 */
		pointLights.emplace_back(PointLightConstants{ {-28.0f, CEILNG_HEIGHT, -13.0f,.0f},{lightColor.red} , POINTLIGHT_RANGE });

		/* OBJなし */
		pointLights.emplace_back(PointLightConstants{ {28.0f, CEILNG_HEIGHT, -23.0f,.0f},{lightColor.red} , POINTLIGHT_RANGE });

		// 黄ライト -------------------------------------------------------------------------------------------------------
		/* 入口 */
		pointLights.emplace_back(PointLightConstants{ {-0.5f, CEILNG_HEIGHT, -24.5f,.0f},{lightColor.yellow} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ { 2.5f, CEILNG_HEIGHT, -24.5f,.0f},{lightColor.yellow} , POINTLIGHT_RANGE });

		// 緑ライト -------------------------------------------------------------------------------------------------------
		/* 研修部屋 */
		pointLights.emplace_back(PointLightConstants{ {4.0f, CEILNG_HEIGHT, 5.0f,.0f},{lightColor.green} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {6.5f, CEILNG_HEIGHT, 5.0f,.0f},{lightColor.green} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {9.0f, CEILNG_HEIGHT, 5.0f,.0f},{lightColor.green} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {4.0f, CEILNG_HEIGHT, 2.0f,.0f},{lightColor.green} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {6.5f, CEILNG_HEIGHT, 2.0f,.0f},{lightColor.green} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {9.0f, CEILNG_HEIGHT, 2.0f,.0f},{lightColor.green} , POINTLIGHT_RANGE });

		/* OBJ緑 */
		pointLights.emplace_back(PointLightConstants{ {-14.0f, CEILNG_HEIGHT, -11.5f,.0f},{lightColor.green} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-11.0f, CEILNG_HEIGHT, -11.5f,.0f},{lightColor.green} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-8.0f,  CEILNG_HEIGHT, -11.5f,.0f},{lightColor.green} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-14.0f, CEILNG_HEIGHT, -14.5f,.0f},{lightColor.green} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-11.0f, CEILNG_HEIGHT, -14.5f,.0f},{lightColor.green} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-8.0f,  CEILNG_HEIGHT, -14.5f,.0f},{lightColor.green} , POINTLIGHT_RANGE });

		// 紫ライト -------------------------------------------------------------------------------------------------------
		/* テレビ部屋 */
		pointLights.emplace_back(PointLightConstants{ {-12.0f, CEILNG_HEIGHT, 6.5f,.0f},{lightColor.purple} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ { -9.0f, CEILNG_HEIGHT, 6.5f,.0f},{lightColor.purple} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-12.0f, CEILNG_HEIGHT, 3.5f,.0f},{lightColor.purple} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ { -9.0f, CEILNG_HEIGHT, 3.5f,.0f},{lightColor.purple} , POINTLIGHT_RANGE });

		/* OBJ紫 */
		pointLights.emplace_back(PointLightConstants{ { 9.5f, CEILNG_HEIGHT, -11.0f,.0f},{lightColor.purple} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {12.5f, CEILNG_HEIGHT, -11.0f,.0f},{lightColor.purple} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ { 9.5f, CEILNG_HEIGHT, -14.0f,.0f},{lightColor.purple} , POINTLIGHT_RANGE });
		pointLights.emplace_back(PointLightConstants{ {12.5f, CEILNG_HEIGHT, -14.0f,.0f},{lightColor.purple} , POINTLIGHT_RANGE });

		// モデルの初期化
		pointLightsModel.clear();
		for (int i = 0; i < POINTLIGHT_MAX; ++i) {
			pointLightsModel.push_back(std::make_unique<LightPoint>(DirectX::XMFLOAT3{ pointLights.at(i).position.x, pointLights.at(i).position.y, pointLights.at(i).position.z }));
		}
	}

	// 円光源
	{
		// 青ライト ------------------------------------------------------------------------------------------------------
		/* 大ライト通路 */
		torusLights.emplace_back(TorusLightConstants{ {1.0f,  CEILNG_HEIGHT, -13.0f,.0f},{0,-1,0,0}, { lightColor.blue },{TORUSLIGHT_MAJOR},{TORUSLIGHT_MINOR },TORUSLIGHT_RANGE });

		// 赤ライト ------------------------------------------------------------------------------------------------------
		/* 大廊下 */
		torusLights.emplace_back(TorusLightConstants{  {-11.0f, CEILNG_HEIGHT, -4.5f,.0f},{0,-1,0,0}, { lightColor.red },{TORUSLIGHT_MAJOR},{TORUSLIGHT_MINOR },TORUSLIGHT_RANGE });
		torusLights.emplace_back(TorusLightConstants{  {  0.0f, CEILNG_HEIGHT, -4.5f,.0f},{0,-1,0,0}, { lightColor.red },{TORUSLIGHT_MAJOR},{TORUSLIGHT_MINOR },TORUSLIGHT_RANGE });
		torusLights.emplace_back(TorusLightConstants{  { 11.0f, CEILNG_HEIGHT, -4.5f,.0f},{0,-1,0,0}, { lightColor.red },{TORUSLIGHT_MAJOR},{TORUSLIGHT_MINOR },TORUSLIGHT_RANGE });

		/* OBJ角 */
		torusLights.emplace_back(TorusLightConstants{ {-28.0f, CEILNG_HEIGHT, -23.0f,.0f},{0,-1,0,0}, { lightColor.red },{TORUSLIGHT_MAJOR},{TORUSLIGHT_MINOR },TORUSLIGHT_RANGE });

		/* OBJなし */
		torusLights.emplace_back(TorusLightConstants{ {28.0f, CEILNG_HEIGHT, -13.0f,.0f},{0,-1,0,0}, { lightColor.red },{TORUSLIGHT_MAJOR},{TORUSLIGHT_MINOR },TORUSLIGHT_RANGE });

		// 黄ライト -------------------------------------------------------------------------------------------------------
        /* 北廊下 */
		torusLights.emplace_back(TorusLightConstants{ {-8.5f, CEILNG_HEIGHT, 14.0f,.0f},{0,-1,0,0}, { lightColor.yellow },{TORUSLIGHT_MAJOR},{TORUSLIGHT_MINOR },TORUSLIGHT_RANGE });
		torusLights.emplace_back(TorusLightConstants{ {8.5f,  CEILNG_HEIGHT, 14.0f,.0f},{0,-1,0,0}, { lightColor.yellow },{TORUSLIGHT_MAJOR},{TORUSLIGHT_MINOR },TORUSLIGHT_RANGE });

		/* 入口 */
		torusLights.emplace_back(TorusLightConstants{ {-5.0f, CEILNG_HEIGHT, -17.0f,.0f},{0,-1,0,0}, { lightColor.yellow },{TORUSLIGHT_MAJOR},{TORUSLIGHT_MINOR },TORUSLIGHT_RANGE });
		torusLights.emplace_back(TorusLightConstants{ { 1.0f, CEILNG_HEIGHT, -22.5f,.0f},{0,-1,0,0}, { lightColor.yellow },{TORUSLIGHT_MAJOR},{TORUSLIGHT_MINOR },TORUSLIGHT_RANGE });
		torusLights.emplace_back(TorusLightConstants{ { 7.0f, CEILNG_HEIGHT, -17.0f,.0f},{0,-1,0,0}, { lightColor.yellow },{TORUSLIGHT_MAJOR},{TORUSLIGHT_MINOR },TORUSLIGHT_RANGE });

		// 緑ライト -------------------------------------------------------------------------------------------------------
		/* 西廊下 */
		torusLights.emplace_back(TorusLightConstants{ {-21.0f, CEILNG_HEIGHT, -4.0f,.0f},{0,-1,0,0}, { lightColor.green },{TORUSLIGHT_MAJOR},{TORUSLIGHT_MINOR },TORUSLIGHT_RANGE });

		// 紫ライト -------------------------------------------------------------------------------------------------------
		/* 東廊下 */
		torusLights.emplace_back(TorusLightConstants{ {21.0f, CEILNG_HEIGHT, -4.5f,.0f},{0,-1,0,0}, { lightColor.purple },{TORUSLIGHT_MAJOR},{TORUSLIGHT_MINOR },TORUSLIGHT_RANGE });

		// モデルの初期化
		torusLightsModel.clear();
		for (int i = 0; i < TORUSLIGHT_MAX; ++i) {
			torusLightsModel.push_back(std::make_unique<LightTorus>(DirectX::XMFLOAT3{ torusLights.at(i).position.x, torusLights.at(i).position.y, torusLights.at(i).position.z }));
		}
	}

	// 線光源の初期化
	{
		// 光源情報の初期化
		lineLights.clear();
		lightData.clear();

		// 青ライト ------------------------------------------------------------------------------------------------------
		/* 最奥部屋 */
		lightData.emplace_back(LightData{ {-8.0f, CEILNG_HEIGHT, 20.0f},0, 1 });
		lightData.emplace_back(LightData{ { 8.0f, CEILNG_HEIGHT, 20.0f},0, 1 });

		/* 大ライト通路 */
		lightData.emplace_back(LightData{ {-3.5f, CEILNG_HEIGHT, -12.0f}, 90, 1 });
		lightData.emplace_back(LightData{ { 5.5f, CEILNG_HEIGHT, -12.0f}, 90, 1 });

		for (int i = 0; i < 4; ++i) {
			lineLights.emplace_back(LineLightConstants{ {0,0,0,0},{0,0,0,0},{lightColor.blue},LINELIGHT_RANGE });
		}

		// 赤ライト ------------------------------------------------------------------------------------------------------
		/* 3席 */
		lightData.emplace_back(LightData{ {-23.0f, CEILNG_HEIGHT, 13.0f},90, 1 });
		lightData.emplace_back(LightData{ {-21.0f, CEILNG_HEIGHT, 9.0f},  0, 1 });
		lightData.emplace_back(LightData{ {-23.0f, CEILNG_HEIGHT, 5.0f}, 90, 1 });

		/* 1席 */
		lightData.emplace_back(LightData{ {24.0f, CEILNG_HEIGHT, 13.0f}, 90, 1 });
		lightData.emplace_back(LightData{ {21.0f, CEILNG_HEIGHT, 9.0f},   0, 1 });
		lightData.emplace_back(LightData{ {24.0f, CEILNG_HEIGHT, 5.0f},  90, 1 });

		/* 大廊下 */
		lightData.emplace_back(LightData{ {-16.0f,CEILNG_HEIGHT, -4.0f}, 90, 1 });
		lightData.emplace_back(LightData{ { -6.0f,CEILNG_HEIGHT, -4.0f}, 90, 1 });
		lightData.emplace_back(LightData{ {  6.0f,CEILNG_HEIGHT, -4.0f}, 90, 1 });
		lightData.emplace_back(LightData{ { 16.0f,CEILNG_HEIGHT, -4.0f}, 90, 1 });

		/* OBJ角 */
		lightData.emplace_back(LightData{ {-23.0f, CEILNG_HEIGHT, -13.0f}, 90, 1 });
		lightData.emplace_back(LightData{ {-21.0f, CEILNG_HEIGHT, -16.0f},  0, 1 });
		lightData.emplace_back(LightData{ {-23.0f, CEILNG_HEIGHT, -23.0f}, 90, 1 });

		/* OBJなし */
		lightData.emplace_back(LightData{ { 24.0f,CEILNG_HEIGHT, -13.0f}, 90, 1 });
		lightData.emplace_back(LightData{ { 21.0f,CEILNG_HEIGHT, -16.0f},0, 1 });
		lightData.emplace_back(LightData{ { 24.0f,CEILNG_HEIGHT, -23.0f}, 90, 1 });

		for (int i = 0; i < 16; ++i) {
			lineLights.emplace_back(LineLightConstants{ {0,0,0,0},{0,0,0,0},{lightColor.red},LINELIGHT_RANGE });
		}

		// 黄ライト -------------------------------------------------------------------------------------------------------
		/* 北廊下 */
		lightData.emplace_back(LightData{ {-15.0f,CEILNG_HEIGHT, 13.0f}, 90, 1 });
		lightData.emplace_back(LightData{ { -3.0f,CEILNG_HEIGHT, 13.0f}, 90, 1 });
		lightData.emplace_back(LightData{ {  3.0f,CEILNG_HEIGHT, 13.0f}, 90, 1 });
		lightData.emplace_back(LightData{ { 15.0f,CEILNG_HEIGHT, 13.0f}, 90, 1 });
		lightData.emplace_back(LightData{ {   .0f,CEILNG_HEIGHT,  8.0f},  0, 1 });
		lightData.emplace_back(LightData{ {   .0f,CEILNG_HEIGHT,  2.0f},  0, 1 });

		/* 入口 */
		lightData.emplace_back(LightData{ {  1.0f,CEILNG_HEIGHT, -17.0f},  0, 1 });
		lightData.emplace_back(LightData{ {-18.0f,CEILNG_HEIGHT, -23.0f}, 90, 1 });
		lightData.emplace_back(LightData{ {-13.0f,CEILNG_HEIGHT, -23.0f}, 90, 1 });
		lightData.emplace_back(LightData{ { -8.0f,CEILNG_HEIGHT, -23.0f}, 90, 1 });
		lightData.emplace_back(LightData{ {  7.0f,CEILNG_HEIGHT, -23.0f}, 90, 1 });
		lightData.emplace_back(LightData{ { 11.0f,CEILNG_HEIGHT, -23.0f}, 90, 1 });
		lightData.emplace_back(LightData{ { 17.0f,CEILNG_HEIGHT, -23.0f}, 90, 1 });
		 
		for (int i = 0; i < 13; ++i) {
			lineLights.emplace_back(LineLightConstants{ {0,0,0,0},{0,0,0,0},{lightColor.yellow},LINELIGHT_RANGE });
		}

		// 緑ライト -------------------------------------------------------------------------------------------------------
		/* 研修部屋 */
		lightData.emplace_back(LightData{ { 12.0f,CEILNG_HEIGHT,  5.0f}, 90, 1 });
		lightData.emplace_back(LightData{ { 17.0f,CEILNG_HEIGHT, 17.0f}, 90, 1 });

		/* 西廊下 */
		lightData.emplace_back(LightData{ {-21.0f,CEILNG_HEIGHT,   1.5f}, 0, 1 });
		lightData.emplace_back(LightData{ {-21.0f,CEILNG_HEIGHT, -10.0f}, 0, 1 });

		/* OBJ緑 */
		lightData.emplace_back(LightData{ {-11.0f,CEILNG_HEIGHT,  -9.0f},  0, 1 });
		lightData.emplace_back(LightData{ {-17.0f,CEILNG_HEIGHT, -13.0f}, 90, 1 });

		for (int i = 0; i < 6; ++i) {
			lineLights.emplace_back(LineLightConstants{ {0,0,0,0},{0,0,0,0},{lightColor.green},LINELIGHT_RANGE });
		}

		// 紫ライト -------------------------------------------------------------------------------------------------------
		/* テレビ部屋 */
		lightData.emplace_back(LightData{ { 16.0f,CEILNG_HEIGHT, 5.0f}, 90, 1 });
		lightData.emplace_back(LightData{ { -4.0f,CEILNG_HEIGHT, 5.0f}, 90, 1 });

		/* 東廊下 */
		lightData.emplace_back(LightData{ { 21.0f,CEILNG_HEIGHT,  1.0f}, 0, 1 });
		lightData.emplace_back(LightData{ { 21.0f,CEILNG_HEIGHT, -9.5f}, 0, 1 });

		/* OBJ紫 */
		lightData.emplace_back(LightData{ { 11.0f, CEILNG_HEIGHT,  -8.0f},  0, 1 });
		lightData.emplace_back(LightData{ { 17.0f ,CEILNG_HEIGHT, -13.0f}, 90, 1 });

		for (int i = 0; i < 6; ++i) {
			lineLights.emplace_back(LineLightConstants{ {0,0,0,0},{0,0,0,0},{lightColor.purple},LINELIGHT_RANGE });
		}

		// モデルの初期化
		lineLightsModel.clear();
		for (int i = 0; i < LINELIGHT_MAX; ++i) {
			lineLightsModel.push_back(std::make_unique<LightBar>(lightData.at(i).position, DirectX::XMConvertToRadians(lightData.at(i).angle)));
		}
	}

	/// モデルの読み込み
	models[0] = std::make_unique<Model>("Data/Model/LightModels/light_circle_assets/light_circle.mdl");
	models[1] = std::make_unique<Model>("Data/Model/LightModels/light_point_assets/light_point.mdl");
	models[2] = std::make_unique<Model>("Data/Model/LightModels/light_bar_low_assets/light_bar_low.mdl");

	/// テクスチャ情報の読み込み
	textures[0] = std::make_unique<LoadTextures>();
	textures[0]->LoadNormal("Data/Model/LightModels/light_circle_assets/textures/light_circle_Normal_DirectX.png");
	textures[0]->LoadRoughness("Data/Model/LightModels/light_circle_assets/textures/light_circle_Roughness.png");
	textures[0]->LoadEmisive("Data/Model/LightModels/light_circle_assets/textures/light_circle_Emissive.png");

	textures[1] = std::make_unique<LoadTextures>();
	textures[1]->LoadNormal("Data/Model/LightModels/light_point_assets/textures/light_point_mtl_Normal_DirectX.png");
	textures[1]->LoadRoughness("Data/Model/LightModels/light_point_assets/textures/light_point_mtl_Roughness.png");
	textures[1]->LoadMetalness("Data/Model/LightModels/light_point_assets/textures/light_point_mtl_Metallic.png");
	textures[1]->LoadEmisive("Data/Model/LightModels/light_point_assets/textures/light_point_mtl_Emissive.png");

	textures[2] = std::make_unique<LoadTextures>();
	textures[2]->LoadRoughness("Data/Model/LightModels/light_bar_low_assets/textures/light_bar_low_Roughness.png");
	textures[2]->LoadEmisive("Data/Model/LightModels/light_bar_low_assets/textures/light_bar_low_Emissive.png");

	float timer = bench.end();
	char buffer[256];
	sprintf_s(buffer, "Time taken to initialize LightManager  : % f\n", timer);
	OutputDebugStringA(buffer);
}

void LightManager::Update()
{
	// ライトのpositionとlengthからstartとendを求める
    for (int i = 0; i < LINELIGHT_MAX; ++i) {

		float radian = DirectX::XMConvertToRadians(lightData.at(i).angle);

        DirectX::XMMATRIX M =
			DirectX::XMMatrixRotationRollPitchYaw(0, radian, 0);
        DirectX::XMVECTOR Dir = M.r[0];
        DirectX::XMFLOAT3 direction;
        DirectX::XMStoreFloat3(&direction, Dir);

        DirectX::XMFLOAT3 start, end;
        //DirectX::XMVECTOR Length = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(&lineLights[i].end), DirectX::XMLoadFloat4(&lineLights[i].start));
        //Length = DirectX::XMVector3Length(Length);
        //float length = DirectX::XMVectorGetX(Length);

        start.x = lightData.at(i).position.x + direction.x * (lightData.at(i).length * 0.5f);
        start.y = lightData.at(i).position.y + direction.y * (lightData.at(i).length * 0.5f);
        start.z = lightData.at(i).position.z + direction.z * (lightData.at(i).length * 0.5f);
        end.x = lightData.at(i).position.x - direction.x * (lightData.at(i).length * 0.5f);
        end.y = lightData.at(i).position.y - direction.y * (lightData.at(i).length * 0.5f);
        end.z = lightData.at(i).position.z - direction.z * (lightData.at(i).length * 0.5f);

        lineLights.at(i).start.x = start.x;
        lineLights.at(i).start.y = start.y;
        lineLights.at(i).start.z = start.z;
        lineLights.at(i).end.x = end.x;
        lineLights.at(i).end.y = end.y;
        lineLights.at(i).end.z = end.z;
    }

	// ライトの色の更新処理
	{
		// 点光源
		for (int i = 0; i < POINTLIGHT_MAX; ++i) {
			if (i < 6) { pointLights.at(i).color       = { lightColor.blue }; }
			else if (i < 12) { pointLights.at(i).color = { lightColor.red }; }
			else if (i < 14) { pointLights.at(i).color = { lightColor.yellow }; }
			else if (i < 26) { pointLights.at(i).color = { lightColor.green }; }
			else if (i < POINTLIGHT_MAX) { pointLights.at(i).color = { lightColor.purple }; }
		}
		// 円光源
		for (int i = 0; i < TORUSLIGHT_MAX; ++i) {
			if (i < 1) { torusLights.at(i).color = { lightColor.blue }; }
			else if (i < 6) { torusLights.at(i).color = { lightColor.red }; }
			else if (i < 11) { torusLights.at(i).color = { lightColor.yellow }; }
			else if (i < 12) { torusLights.at(i).color = { lightColor.green }; }
			else if (i < TORUSLIGHT_MAX) { torusLights.at(i).color = { lightColor.purple }; }
		}
		// 線光源
		for (int i = 0; i < LINELIGHT_MAX; ++i) {
			if (i < 4) { lineLights.at(i).color       = { lightColor.blue }; }
			else if (i < 20) { lineLights.at(i).color = { lightColor.red }; }
			else if (i < 33) { lineLights.at(i).color = { lightColor.yellow }; }
			else if (i < 39) { lineLights.at(i).color = { lightColor.green }; }
			else if (i < LINELIGHT_MAX) { lineLights.at(i).color = { lightColor.purple }; }
		}
	}
}

void LightManager::Render(RenderContext& rc)
{
	Graphics& graphics = Graphics::Instance(); ///< グラフィックス管理インスタンス
	ID3D11DeviceContext* dc = graphics.GetDeviceContext(); ///< デバイスコンテキスト
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();

	/// 点光源のモデル描画
	for (auto& p : pointLightsModel) {
		p->Render(rc, modelRenderer, models[1].get(), textures[1].get());
	}
	/// 円光源のモデル描画
	for (auto& p : torusLightsModel) {
		p->Render(rc, modelRenderer, models[0].get(), textures[0].get());
	}
	/// 線光源のモデル描画
	for (auto& p : lineLightsModel) {
		p->Render(rc, modelRenderer, models[2].get(), textures[2].get());
	}
}

void LightManager::RenderDebugPrimitive(RenderContext& rc)
{
	Graphics& graphics = Graphics::Instance(); ///< グラフィックス管理インスタンス
	ID3D11DeviceContext* dc = graphics.GetDeviceContext(); ///< デバイスコンテキスト
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer(); ///< 形状描画用レンダラー

	// 点光源のデバッグ描画
	for (auto& p : pointLights) {
		shapeRenderer->RenderSphere(rc, { p.position.x, p.position.y, p.position.z }, 0.2f, { 1, 1, 0, 1 });
	}
	// 円光源のデバッグ描画
	for (auto& p : torusLights) {
		shapeRenderer->RenderSphere(rc, { p.position.x, p.position.y, p.position.z }, 0.4f, { 1, 0, 0, 1 });
	}
	/// 線光源のデバッグ描画
	for (auto& p : lightData) {
		shapeRenderer->RenderBox(rc, p.position, { 0,DirectX::XMConvertToRadians(p.angle) ,0 }, { 1,0.4f,0.4f }, { 0,0,1,1 });
	}
}

void LightManager::DebugGUI()
{
	if (ImGui::TreeNode("Light"))
	{
		ImGui::DragFloat("lightPower", &lightPower, 0.1f, 0, 100);
		if (ImGui::TreeNode("lightsColor"))
		{
			ImGui::ColorEdit3("blue",   &lightColor.blue.x);
			ImGui::ColorEdit3("red",    &lightColor.red.x);
			ImGui::ColorEdit3("yellow", &lightColor.yellow.x);
			ImGui::ColorEdit3("green",  &lightColor.green.x);
			ImGui::ColorEdit3("purple", &lightColor.purple.x);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("pointLight"))
		{
			PointLightDebug("innermost room(blue)",       0,  3);
			PointLightDebug("large light corridor(blue)", 4,  5);

			PointLightDebug("three seats room(red)",      6,  7);
			PointLightDebug("one seats room(red)",        8, 9);
			PointLightDebug("OBJ corner(red)",           10, 10);
			PointLightDebug("No OBJ(red)",               11, 11);

			PointLightDebug("entrance(yellow)",          12, 13);

			PointLightDebug("Laboratory(green)",         14, 19);
			PointLightDebug("OBJ green(green)",          20, 25);

			PointLightDebug("TV room(purple)",           26, 29);
			PointLightDebug("OBJ purple(purple)",        30, POINTLIGHT_MAX - 1);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("torusLights"))
		{
			TorusLightDebug("large light corridor(blue)", 0, 0);

			TorusLightDebug("large corridor(red)", 1, 3);
			TorusLightDebug("OBJ corner(red)", 4, 4);
			TorusLightDebug("No OBJ(red)", 5, 5);

			TorusLightDebug("north corridor(yellow)", 6, 7);
			TorusLightDebug("entrance(yellow)", 8, 10);

			TorusLightDebug("west corridor(green)", 11, 11);

			TorusLightDebug("east corridor(purple)", 12, 12);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("lineLight"))
		{
			LineLightDebug("innermost room(blue)",       0, 1);
			LineLightDebug("large light corridor(blue)", 2, 3);

			LineLightDebug("three seats room(red)",      4,  6);
			LineLightDebug("one seats room(red)",        7,  9);
			LineLightDebug("large corridor(red)",       10, 13);
			LineLightDebug("OBJ corner(red)",           14, 16);
			LineLightDebug("No OBJ(red)",               17, 19);

			LineLightDebug("north corridor(yellow)",    20, 25);
			LineLightDebug("entrance(yellow)",          26, 32);

			LineLightDebug("Laboratory(green)",         33, 34);
			LineLightDebug("west corridor(green)",      35, 36);
			LineLightDebug("OBJ green(green)",          37, 38);

			LineLightDebug("TV room(purple)",           39, 40);
			LineLightDebug("east corridor(purple)",     41, 42);
			LineLightDebug("OBJ purple(purple)",        443, LINELIGHT_MAX - 1);			

			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void LightManager::UpdateConstants(RenderContext& rc)
{
	// ポイントライトの設定
	rc.lightPower = lightPower;
	for (int i = 0; i < POINTLIGHT_MAX; ++i) {
		rc.pointLights[i].position = pointLights.at(i).position;
		rc.pointLights[i].color    = pointLights.at(i).color;
		rc.pointLights[i].range    = pointLights.at(i).range;
		//rc.pointLights[i].dummy = { 0,0,0 };
	}
	for (int i = 0; i < TORUSLIGHT_MAX; ++i) {
		rc.torusLights[i].position = torusLights.at(i).position;
		rc.torusLights[i].direction = torusLights.at(i).direction;
		rc.torusLights[i].color = torusLights.at(i).color;
		rc.torusLights[i].majorRadius = torusLights.at(i).majorRadius;
		rc.torusLights[i].minorRadius = torusLights.at(i).minorRadius;
		rc.torusLights[i].range = torusLights.at(i).range;
		//rc.pointLights[i].dummy = { 0,0,0 };
	}
	for (int i = 0; i < LINELIGHT_MAX; ++i) {
		rc.lineLights[i].start = lineLights.at(i).start;
		rc.lineLights[i].end   = lineLights.at(i).end;
		rc.lineLights[i].color = lineLights.at(i).color;
		rc.lineLights[i].range = lineLights.at(i).range;
		//rc.lineLights[i].dummy = { 0,0,0 };
	}
}

void LightManager::PointLightDebug(char* c,int begin, int end)
{
	if (ImGui::TreeNode(c))
	{
		for (int i = begin; i <= end; ++i) {
			if (pointLights.at(i).range == POINTLIGHT_RANGE) { ImGui::Text("small light"); }
			else if (pointLights.at(i).range == TORUSLIGHT_RANGE) { ImGui::Text("large light"); }

			std::string p = std::string("position") + std::to_string(i);
			ImGui::DragFloat3(p.c_str(), &pointLights.at(i).position.x, 0.1f, -30.0f, +30.0f);
			std::string r = std::string("range") + std::to_string(i);
			ImGui::DragFloat(r.c_str(), &pointLights.at(i).range, 0.1f, 0.0f, +100.0f);
		}

		ImGui::TreePop();
	}
}

void LightManager::TorusLightDebug(char* c, int begin, int end)
{
	if (ImGui::TreeNode(c))
	{
		for (int i = begin; i <= end; ++i) {
			std::string p = std::string("position") + std::to_string(i);
			ImGui::DragFloat3(p.c_str(), &torusLights.at(i).position.x, 0.1f, -30.0f, +30.0f);
			std::string r = std::string("majorRadius") + std::to_string(i);
			ImGui::DragFloat(r.c_str(), &torusLights.at(i).majorRadius, 0.1f, 0.0f, +10);
			r = std::string("minorRadius") + std::to_string(i);
			ImGui::DragFloat(r.c_str(), &torusLights.at(i).minorRadius, 0.1f, 0.0f, +10);
			r = std::string("range") + std::to_string(i);
			ImGui::DragFloat(r.c_str(), &torusLights.at(i).range, 0.1f, 0.0f, +100.0f);
		}

		ImGui::TreePop();
	}
}

void LightManager::LineLightDebug(char* c, int begin, int end)
{
	if (ImGui::TreeNode(c))
	{
		for (int i = begin; i <= end; ++i) {
			std::string s = std::string("pos") + std::to_string(i);
			ImGui::DragFloat3(s.c_str(), &lightData.at(i).position.x, 0.1f, -30.0f, +30.0f);
			std::string l = std::string("length") + std::to_string(i);
			ImGui::DragFloat(l.c_str(), &lightData.at(i).length, 0.1f, -10.0f, +10.0f);
			std::string a = std::string("angle") + std::to_string(i);
			ImGui::DragFloat(a.c_str(), &lightData.at(i).angle, 1.0f, 0, +90.0f);
			std::string r = std::string("range") + std::to_string(i);
			ImGui::DragFloat(r.c_str(), &lineLights.at(i).range, 0.1f, 0.0f, +100.0f);
		}
		ImGui::TreePop();
	}
}
