#pragma once
#include <memory>
#include "Stage.h"
#include "ICameraController.h"
#include "FPCameraController.h"
#include "FreeCameraController.h"
#include "LightDebugCameraController.h"
#include "Scene.h"
#include "Player/Player.h"
#include "Enemy/Enemy.h"
#include"miniMap.h"

#include "System/ShadowCaster.h"

// ゲームシーン
class fujimoto :public Scene
{
public:
	fujimoto() {}
	~fujimoto() override {}

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

	void UpdateCamera(float elapsedTime);

private:
	Stage* stage = nullptr;

	std::unique_ptr<ICameraController> i_CameraController = nullptr;
	std::shared_ptr<Player> player;
	std::shared_ptr<Enemy> enemy;
	MiniMap* minimap = nullptr;
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
	DirectX::XMFLOAT3 shadowColor = { 0.89f, 0.89f, 0.89f };

	// シャドウマップ描画範囲（デバッグ描画用？）
	float SHADOWMAP_DRAWRECT = 30.0f;

	// シャドウマップ描画・生成を担当するクラス
	std::unique_ptr<ShadowCaster> shadow;


	// ==============================
	// フォグ・環境光関連定数
	// ==============================

	// アンビエントライトの色（暗めのグレー）
	DirectX::XMFLOAT4 ambientColor = { 0.25f, 0.273f, 0.335f, 1.0f };

	// フォグの色（白に近いグレー）
	DirectX::XMFLOAT4 fogColor = { .0f,.0f,.0f, 1.0f };

	// フォグの範囲（開始20.0、終了100.0）
	DirectX::XMFLOAT4 fogRange = { 10.0f, 50.0f, 0, 0 };


	// ==============================
	// カメラ・ライト方向
	// ==============================

	// 平行光源の方向ベクトル（斜め上から照射）
	DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, 1.0f };

	// カメラのワールド座標
	DirectX::XMFLOAT3 cameraPosition = { 0.0f, 0.0f, 0.0f };
	void UpdateConstants(RenderContext& rc);
};