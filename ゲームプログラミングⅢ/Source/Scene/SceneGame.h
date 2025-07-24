#pragma once
#include <memory>
#include "Stage.h"
#include "ICameraController.h"
#include "FPCameraController.h"
#include "FreeCameraController.h"
#include "LightDebugCameraController.h"
#include "Scene.h"
#include <memory>
#include "Player/Player.h"
#include"miniMap.h"
#include "3DAudio/3DAudio.h"
#include"Metar.h"

#include "System/ShadowCaster.h"
#include "System/UiManager.h"

extern float reminingTime;
// ゲームシーン
class SceneGame :public Scene
{
public:
	SceneGame() {}
	SceneGame(int Life):life_number(Life){}
	~SceneGame() override {}

	// 初期化
	void Initialize()override;

	// 終了化
	void Finalize()override;

	// 更新処理
	void Update(float elapsedTime)override;

	// 描画処理
	void Render()override;

	// GUI描画
	void DrawGUI()override;

private:
	void Collision();

	void PlayerVsStage();

	void PlayerVsEnemy();

	void UpdateCamera(float elapsedTime);

	void UpdateOneWay(float elapsedTime);

	void TutorialUpdate(float elapsedTime);

	bool IsPlayerFacingDoor(const DirectX::XMFLOAT3& playerPos, const DirectX::XMFLOAT3& playerDir, const DirectX::XMFLOAT3& doorPos, float threshold);

	void CheckGateInteraction(std::shared_ptr<Player> player, Stage* stage, bool& fadeStart);

private:
	Stage* stage = nullptr;

	std::unique_ptr<ICameraController> i_CameraController = nullptr;
	std::shared_ptr<Player> player;
	std::shared_ptr<Enemy> enemy;
	std::shared_ptr<Metar> metar;
	MiniMap* minimap = nullptr;
	bool tutorial_Flug = false;
	int tutorial_Step = 0;
	float tutorialTimer = 0.0f;
	int tutorial_Click_Count = 0;
	float button_effect_timer = 0;
	float button_effect = 0.5f;
	float timer = 0.0f; // タイマー
	float transTimer = 0.0f; // シーン遷移タイマー
	bool sceneTrans = false;
	Scene* nextScene = nullptr; ///< 1秒後に遷移するシーン
	enum class SelectTrans {
		Clear, // シーンクリア
		GameOver, // ゲームオーバー
		cnt,
	};
	SelectTrans selectTrans = SelectTrans::GameOver; // シーン遷移選択
	// ==============================
	// シャドウ関連定数
	// ==============================

	// ライト視点からのビュー射影行列（シャドウマップ作成用）
	DirectX::XMFLOAT4X4 lightViewProjection;

	// シャドウの深度バイアス（アクネ防止）
	float shadowBias = 0.001f;

	// 影の色（やや明るめのグレー）
	DirectX::XMFLOAT3 shadowColor = { 0.7812f,0.7812f,0.7812f };
	DirectX::XMFLOAT4 edgeColor = { .0f,.0f,.0f,1.0f };

	// シャドウマップ描画範囲（デバッグ描画用？）
	float SHADOWMAP_DRAWRECT = 30.0f;

	// シャドウマップ描画・生成を担当するクラス
	std::unique_ptr<ShadowCaster> shadow;


	// ==============================
	// フォグ・環境光関連定数
	// ==============================

	// アンビエントライトの色（暗めのグレー）
	DirectX::XMFLOAT4 ambientColor = { 0.1093f, 0.1093f, 0.1093f, 1.0f };

	// フォグの色（白に近いグレー）
	DirectX::XMFLOAT4 fogColor = { .0f,.0f,.0f, 1.0f };

	// フォグの範囲（開始20.0、終了100.0）
	DirectX::XMFLOAT4 fogRange = { 10.0f, 30.0f, 0, 0 };

	// ==============================
	// カメラ・ライト方向
	// ==============================

	// 平行光源の方向ベクトル（斜め上から照射）
	DirectX::XMFLOAT3 lightDirection = { 0.0f, -3.0f, 0.0f };

	// カメラのワールド座標
	DirectX::XMFLOAT3 cameraPosition = { 0.0f, 0.0f, 0.0f };
	void UpdateConstants(RenderContext& rc);


	// 3Dオーディオシステム

	Audio3DSystem audioSystem; ///< 3Dオーディオシステムのインスタンス


	int life_number;

	// ======================================
	// チュ－トリアルのスプライト配列
	// ======================================
	std::unique_ptr<Sprite> tutorial[13];

	UIManager um;

	float fadeTime;
	bool fadeStart;
	const float totalFadeTime = 0.5;
	int selectDoorIndex = -1;
};