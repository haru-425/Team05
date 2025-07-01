#pragma once
#include <memory>
#include "Stage.h"
#include "ICameraController.h"
#include "FPCameraController.h"
#include "FreeCameraController.h"
#include "Scene.h"
#include <memory>
#include "Player/Player.h"
#include "System/ShadowCaster.h"

// ゲームシーン
class SceneGraphics :public Scene
{
public:
	SceneGraphics() {}
	~SceneGraphics() override {}

	// ライト関連の初期化
	void SetPointLightData();

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

	// 定数の更新
	void UpdateConstants(RenderContext& rc);

private:
	Stage* stage = nullptr;

	std::unique_ptr<ICameraController> i_CameraController = nullptr;
	std::shared_ptr<Player> player;

private:
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


	// ==============================
	// 点光源・線光源の設定
	// ==============================

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
		DirectX::XMFLOAT3 dummy = { 0,0,0 };  // 16バイトアラインメント調整用
	};

	// 光源位置・向きを格納する汎用データ（カスタム用途など）
	struct LightData
	{
		DirectX::XMFLOAT3 position;  // ライトの位置
		DirectX::XMFLOAT3 angle;     // 向きや回転（未使用ならゼロ）
		float             length;    // 光源の長さ
	};

	// 光源情報
	LightData lightData[8] = {};              // 汎用光源データ（初期化済み）
	PointLightConstants pointLights[8];       // 最大8つの点光源
	LineLightConstants lineLights[8];         // 最大8つの線光源

	// 全体のライト強度（乗算係数）
	float lightPower = 5.0f;
};
