#pragma once

#include "System/Sprite.h"
#include "Scene.h"
#include "System/Model.h"
#include "System/ModelRenderer.h"
#include <memory>
#include <vector>
#include "System/ShadowCaster.h"
#include "Camera/CameraController/ICameraController.h"
#include "Camera/CameraController/FreeCameraController.h"
#include "Stage.h"
#include "System/UI.h"
#include "System/UiManager.h"
#include "3DAudio/3DAudio.h"

//タイトルシーン
class SceneTitle :public Scene
{
public:
	SceneTitle() {}
	~SceneTitle()override {}

	//初期化
	void Initialize()override;

	//終了化
	void Finalize()override;

	//更新処理
	void Update(float elapsedTime)override;

	//描画処理
	void Render()override;

	//GUI描画
	void DrawGUI()override;

private:
	void UpdateTransform();

	void UpdateConstants(RenderContext& rc);

	void RenderUI(const RenderContext& rc); ///< タイトルのUIをまとめて描画

	void UpdateUI();

private:
	struct UIParameter
	{
		DirectX::XMFLOAT3 position;
		float dw;
		float dh;
		float angle;
		float sx;
		float sy;
		float sw;
		float sh;
		DirectX::XMFLOAT4 color = { 1,1,1,1 };
	};

private:
	Sprite* sprite = nullptr;
	float TitleTimer = 0.0f; // タイトル画面のタイマー
	float TitleSignalTimer = 0.0f; // タイトル画面の信号タイマー
	bool sceneTrans = false;
	Scene* nextScene = nullptr; ///< 1秒後に遷移するシーン

	std::unique_ptr<Stage> model;

	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT4X4 world;

	std::unique_ptr<ICameraController> i_cameraController;

	/// UIスプライト関連
	UIParameter uiParam[7] = {};
	std::vector<std::unique_ptr<Sprite>> uiSprits;
	std::vector<std::unique_ptr<UI>> ui;
	UIManager um;
	bool isVolumeSliderActive = false;

	/// シェーダ関連
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

	float GraphicsScenetime = 0; //gameタイマー

	bool isStartGame = false;
};
