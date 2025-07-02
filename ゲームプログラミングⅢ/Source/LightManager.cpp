#include "LightManager.h"

#include <imgui.h>
#include "System/Graphics.h"
#include <iostream>

void LightManager::Initialize()
{
	// ライトの強さ
	lightPower = 5;

	// 点光源の初期化
	{
		pointLights.clear();

		// 青ライト ------------------------------------------------------------------------------------------------------
		/* 最奥部屋 */
		pointLights.emplace_back(PointLightConstants{ {-1.3f, CEILNG_HEIGHT, 23.5f,.0f},{.0f, .0f, 1.0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-1.3f, CEILNG_HEIGHT, 20.5f,.0f},{.0f, .0f, 1.0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {2.3f,  CEILNG_HEIGHT, 23.5f,.0f},{.0f, .0f, 1.0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {2.3f,  CEILNG_HEIGHT, 20.5f,.0f},{.0f, .0f, 1.0f,1.0f} , POINTLIGHT_S_RANGE });

		/* 大ライト通路 */
		pointLights.emplace_back(PointLightConstants{ {-0.1f, CEILNG_HEIGHT, -10.8f,.0f},{.0f, .0f, 1.0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {2.25f, CEILNG_HEIGHT, -10.8f,.0f},{.0f, .0f, 1.0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {1.0f,  CEILNG_HEIGHT, -13.0f,.0f},{.0f, .0f, 1.0f,1.0f} , POINTLIGHT_L_RANGE });  // 大ライト

		// 赤ライト ------------------------------------------------------------------------------------------------------
	   /* 3席 */
		pointLights.emplace_back(PointLightConstants{ {-29.0f, CEILNG_HEIGHT, -13.0f,.0f},{1.0f, .0f, .0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-29.0f, CEILNG_HEIGHT, 5.0f,.0f},{1.0f, .0f, .0f,1.0f} , POINTLIGHT_S_RANGE });

		/* 1席 */
		pointLights.emplace_back(PointLightConstants{ { 29.0f, CEILNG_HEIGHT, 13.0f,.0f},{1.0f, .0f, .0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ { 29.0f, CEILNG_HEIGHT, 5.0f,.0f},{1.0f, .0f, .0f,1.0f} , POINTLIGHT_S_RANGE });

		/* 大廊下 */
		pointLights.emplace_back(PointLightConstants{ {-11.0f, CEILNG_HEIGHT, -4.5f,.0f},{1.0f, .0f, .0f,1.0f} , POINTLIGHT_L_RANGE });  // 大ライト
		pointLights.emplace_back(PointLightConstants{ {  0.0f, CEILNG_HEIGHT, -4.5f,.0f},{1.0f, .0f, .0f,1.0f} , POINTLIGHT_L_RANGE });  // 大ライト
		pointLights.emplace_back(PointLightConstants{ { 11.0f, CEILNG_HEIGHT, -4.5f,.0f},{1.0f, .0f, .0f,1.0f} , POINTLIGHT_L_RANGE });  // 大ライト

		/* OBJ角 */
		pointLights.emplace_back(PointLightConstants{ {-28.0f, CEILNG_HEIGHT, -13.0f,.0f},{1.0f, .0f, .0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-28.0f, CEILNG_HEIGHT, -23.0f,.0f},{1.0f, .0f, .0f,1.0f} , POINTLIGHT_L_RANGE });  // 大ライト

		/* OBJなし */
		pointLights.emplace_back(PointLightConstants{ {28.0f, CEILNG_HEIGHT, 23.0f,.0f},{1.0f, .0f, .0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {28.0f, CEILNG_HEIGHT, -13.0f,.0f},{1.0f, .0f, .0f,1.0f} , POINTLIGHT_L_RANGE });  // 大ライト

		// 黄ライト -------------------------------------------------------------------------------------------------------
		/* 北廊下 */
		pointLights.emplace_back(PointLightConstants{ {-8.5f, CEILNG_HEIGHT, 14.0f,.0f},{1.0f, 1.0f, .0f,1.0f} , POINTLIGHT_L_RANGE });  // 大ライト
		pointLights.emplace_back(PointLightConstants{ {8.5f,  CEILNG_HEIGHT, 14.0f,.0f},{1.0f, 1.0f, .0f,1.0f} , POINTLIGHT_L_RANGE });  // 大ライト

		/* 入口 */
		pointLights.emplace_back(PointLightConstants{ {-0.5f, CEILNG_HEIGHT, -24.5f,.0f},{1.0f, 1.0f, .0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ { 2.5f, CEILNG_HEIGHT, -24.5f,.0f},{1.0f, 1.0f, .0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-5.0f, CEILNG_HEIGHT, -17.0f,.0f},{1.0f, 1.0f, .0f,1.0f} , POINTLIGHT_L_RANGE });  // 大ライト
		pointLights.emplace_back(PointLightConstants{ { 1.0f, CEILNG_HEIGHT, -22.5f,.0f},{1.0f, 1.0f, .0f,1.0f} , POINTLIGHT_L_RANGE });  // 大ライト
		pointLights.emplace_back(PointLightConstants{ { 7.0f, CEILNG_HEIGHT, -17.0f,.0f},{1.0f, 1.0f, .0f,1.0f} , POINTLIGHT_L_RANGE });  // 大ライト

		// 緑ライト -------------------------------------------------------------------------------------------------------
		/* 研修部屋 */
		pointLights.emplace_back(PointLightConstants{ {4.0f, CEILNG_HEIGHT, 5.0f,.0f},{.0f, 1.0f, .0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {6.5f, CEILNG_HEIGHT, 5.0f,.0f},{.0f, 1.0f, .0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {9.0f, CEILNG_HEIGHT, 5.0f,.0f},{.0f, 1.0f, .0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {4.0f, CEILNG_HEIGHT, 2.0f,.0f},{.0f, 1.0f, .0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {6.5f, CEILNG_HEIGHT, 2.0f,.0f},{.0f, 1.0f, .0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {9.0f, CEILNG_HEIGHT, 2.0f,.0f},{.0f, 1.0f, .0f,1.0f} , POINTLIGHT_S_RANGE });

		/* 西廊下 */
		pointLights.emplace_back(PointLightConstants{ {-21.0f, CEILNG_HEIGHT, -4.0f,.0f},{.0f, 1.0f, .0f,1.0f} , POINTLIGHT_L_RANGE });  // 大ライト

		/* OBJ緑 */
		pointLights.emplace_back(PointLightConstants{ {-14.0f, CEILNG_HEIGHT, -11.5f,.0f},{.0f, 1.0f, .0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-11.0f, CEILNG_HEIGHT, -11.5f,.0f},{.0f, 1.0f, .0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-8.0f,  CEILNG_HEIGHT, -11.5f,.0f},{.0f, 1.0f, .0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-14.0f, CEILNG_HEIGHT, -14.5f,.0f},{.0f, 1.0f, .0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-11.0f, CEILNG_HEIGHT, -14.5f,.0f},{.0f, 1.0f, .0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-8.0f,  CEILNG_HEIGHT, -14.5f,.0f},{.0f, 1.0f, .0f,1.0f} , POINTLIGHT_S_RANGE });

		// 紫ライト -------------------------------------------------------------------------------------------------------
		/* テレビ部屋 */
		pointLights.emplace_back(PointLightConstants{ {-12.0f, CEILNG_HEIGHT, 6.5f,.0f},{1.0f, .0f, 1.0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ { -9.0f, CEILNG_HEIGHT, 6.5f,.0f},{1.0f, .0f, 1.0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {-12.0f, CEILNG_HEIGHT, 3.5f,.0f},{1.0f, .0f, 1.0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ { -9.0f, CEILNG_HEIGHT, 3.5f,.0f},{1.0f, .0f, 1.0f,1.0f} , POINTLIGHT_S_RANGE });

		/* 東廊下 */
		pointLights.emplace_back(PointLightConstants{ {21.0f, CEILNG_HEIGHT, -4.5f,.0f},{1.0f, .0f, 1.0f,1.0f} , POINTLIGHT_L_RANGE });  // 大ライト

		/* OBJ紫 */
		pointLights.emplace_back(PointLightConstants{ { 9.5f, CEILNG_HEIGHT, -11.0f,.0f},{1.0f, .0f, 1.0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {12.5f, CEILNG_HEIGHT, -11.0f,.0f},{1.0f, .0f, 1.0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ { 9.5f, CEILNG_HEIGHT, -14.0f,.0f},{1.0f, .0f, 1.0f,1.0f} , POINTLIGHT_S_RANGE });
		pointLights.emplace_back(PointLightConstants{ {12.5f, CEILNG_HEIGHT, -14.0f,.0f},{1.0f, .0f, 1.0f,1.0f} , POINTLIGHT_S_RANGE });
	}


	// 線光源の初期化
	{
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
			lineLights.emplace_back(LineLightConstants{ {0,0,0,0},{0,0,0,0},{0,0,1,1},LINELIGHT_RANGE });
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
		lightData.emplace_back(LightData{ {-21.0f, CEILNG_HEIGHT, -16.00f}, 0, 1 });

		/* OBJなし */
		lightData.emplace_back(LightData{ { 21.0f,CEILNG_HEIGHT, -17.0f},0, 1 });
		lightData.emplace_back(LightData{ { 24.0f,CEILNG_HEIGHT, -23.0f}, 90, 1 });

		for (int i = 0; i < 14; ++i) {
			lineLights.emplace_back(LineLightConstants{ {0,0,0,0},{0,0,0,0},{1,0,0,1},LINELIGHT_RANGE });
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
		lightData.emplace_back(LightData{ {-17.0f,CEILNG_HEIGHT, -23.0f}, 90, 1 });
		lightData.emplace_back(LightData{ { -9.0f,CEILNG_HEIGHT, -23.0f}, 90, 1 });
		lightData.emplace_back(LightData{ {  7.0f,CEILNG_HEIGHT, -23.0f}, 90, 1 });
		lightData.emplace_back(LightData{ { 11.0f,CEILNG_HEIGHT, -23.0f}, 90, 1 });
		lightData.emplace_back(LightData{ { 17.0f,CEILNG_HEIGHT, -23.0f}, 90, 1 });
		 
		for (int i = 0; i < 12; ++i) {
			lineLights.emplace_back(LineLightConstants{ {0,0,0,0},{0,0,0,0},{1,1,0,1},LINELIGHT_RANGE });
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
			lineLights.emplace_back(LineLightConstants{ {0,0,0,0},{0,0,0,0},{0,1,0,1},LINELIGHT_RANGE });
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
			lineLights.emplace_back(LineLightConstants{ {0,0,0,0},{0,0,0,0},{1,0,1,1},LINELIGHT_RANGE });
		}
	}
}

void LightManager::Update()
{

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
}

void LightManager::RenderDebug(RenderContext& rc)
{
    Graphics& graphics = Graphics::Instance(); ///< グラフィックス管理インスタンス
    ID3D11DeviceContext* dc = graphics.GetDeviceContext(); ///< デバイスコンテキスト
    ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer(); ///< 形状描画用レンダラー

    /// 点光源のデバッグ描画
    for (auto& p : pointLights) {
		if (p.range == POINTLIGHT_S_RANGE) {
			shapeRenderer->RenderSphere(rc, { p.position.x, p.position.y, p.position.z }, 0.2f, { 1, 1, 0, 1 });
		}
		else if (p.range == POINTLIGHT_L_RANGE) {
			shapeRenderer->RenderSphere(rc, { p.position.x, p.position.y, p.position.z }, 0.5f, { 1, 0, 0, 1 });
		}
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
		ImGui::SliderFloat("lightPower", &lightPower, 0, 100);
		if (ImGui::TreeNode("pointLight"))
		{
			for (int i = 0; i < POINTLIGHT_MAX; ++i) {

				if (i == 0) { ImGui::Text("innermost room"); }
				if (i == 4) { ImGui::Text("large light corridor"); }

				if (i == 7) { ImGui::Text("three seats room"); }
				if (i == 9) { ImGui::Text("one seats room"); }
				if (i == 11) { ImGui::Text("large corridor"); }
				if (i == 14) { ImGui::Text("OBJ corner"); }
				if (i == 16) { ImGui::Text("No OBJ"); }

				if (i == 18) { ImGui::Text("south corridor"); }
				if (i == 20) { ImGui::Text("entrance"); }

				if (i == 25) { ImGui::Text("Laboratory"); }
				if (i == 31) { ImGui::Text("west corridor"); }
				if (i == 32) { ImGui::Text("OBJ green"); }

				if (i == 38) { ImGui::Text("TV room"); }
				if (i == 42) { ImGui::Text("east corridor"); }
				if (i == 43) { ImGui::Text("OBJ purple"); }


				if (pointLights.at(i).range >= POINTLIGHT_L_RANGE) { ImGui::Text("large light"); }

				std::string p = std::string("position") + std::to_string(i);
				ImGui::DragFloat3(p.c_str(), &pointLights.at(i).position.x, 0.1f, -30.0f, +30.0f);
				std::string c = std::string("color") + std::to_string(i);
				ImGui::ColorEdit3(c.c_str(), &pointLights.at(i).color.x);
				std::string r = std::string("range") + std::to_string(i);
				ImGui::DragFloat(r.c_str(), &pointLights.at(i).range, 0.1f, 0.0f, +100.0f);
			}

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("lineLight"))
		{

			for (int i = 0; i < LINELIGHT_MAX; ++i) 
			{
				if (i == 0) { ImGui::Text("innermost room"); }
				if (i == 2) { ImGui::Text("large light corridor"); }

				if (i == 4) { ImGui::Text("three seats room"); }
				if (i == 7) { ImGui::Text("one seats room"); }
				if (i == 10) { ImGui::Text("large corridor"); }
				if (i == 14) { ImGui::Text("OBJ corner"); }
				if (i == 16) { ImGui::Text("No OBJ"); }

				if (i == 18) { ImGui::Text("south corridor"); }
				if (i == 24) { ImGui::Text("entrance"); }

				if (i == 30) { ImGui::Text("Laboratory"); }
				if (i == 32) { ImGui::Text("west corridor"); }
				if (i == 34) { ImGui::Text("OBJ green"); }

				if (i == 36) { ImGui::Text("TV room"); }
				if (i == 38) { ImGui::Text("east corridor"); }
				if (i == 40) { ImGui::Text("OBJ purple"); }

				std::string s = std::string("pos") + std::to_string(i);
				ImGui::DragFloat3(s.c_str(), &lightData.at(i).position.x, 0.1f, -30.0f, +30.0f);
				std::string l = std::string("length") + std::to_string(i);
				ImGui::DragFloat(l.c_str(), &lightData.at(i).length, 0.1f, -10.0f, +10.0f);
				std::string a = std::string("angle") + std::to_string(i);
				ImGui::DragFloat(a.c_str(), &lightData.at(i).angle, 1.0f, 0, +90.0f);
				std::string c = std::string("color") + std::to_string(i);
				ImGui::ColorEdit3(c.c_str(), &lineLights.at(i).color.x);
				std::string r = std::string("range") + std::to_string(i);
				ImGui::DragFloat(r.c_str(), &lineLights.at(i).range, 0.1f, 0.0f, +100.0f);
			}

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
	for (int i = 0; i < LINELIGHT_MAX; ++i) {
		rc.lineLights[i].start = lineLights.at(i).start;
		rc.lineLights[i].end   = lineLights.at(i).end;
		rc.lineLights[i].color = lineLights.at(i).color;
		rc.lineLights[i].range = lineLights.at(i).range;
		//rc.lineLights[i].dummy = { 0,0,0 };
	}
}
